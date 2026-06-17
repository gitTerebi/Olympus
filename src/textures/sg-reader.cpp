#include "sg-reader.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "game-dir.h"
#include "spriteData/espritedata.h"
#include "composite-templates.h"

namespace {

// SG3 layout (verified against Zeus + Poseidon DATA/, citybuilding-tools wiki):
//   fixed image-record table start, constant across files/versions.
//   record size 72 for version >= 214 (alpha variant), 64 for 213.
constexpr int kRecordStart = 40680;   // 80 header + 600 index + 200*200 bitmap reserve
constexpr int kRecSz214    = 72;
constexpr int kRecSz213    = 64;
constexpr uint16_t kTransparent = 0xf81f;

struct SgRecord {
    uint32_t offset = 0;       // byte offset into the .555 pixel blob
    uint32_t dataLength = 0;   // compressed/raw byte length
    uint16_t width = 0;
    uint16_t height = 0;
    bool compressed = false;
    bool isometric = false;    // type@50 == 30: footprint-diamond layout (terrain)
    uint32_t alphaOffset = 0;  // v214: byte offset of RLE alpha mask in the .555 blob
    uint32_t alphaLength = 0;  // v214: alpha-mask byte length (0 = no mask)
};

// One opened .sg3 + its .555 pixel blob, parsed once and cached for the session.
struct SgFile {
    std::vector<SgRecord> fRecords;   // 1-based record table (index 0 unused/empty)
    std::vector<uint8_t> fPixels;     // entire .555 file
    // Sub-group name (bitmap, lowercased, ".bmp" stripped) -> global record start.
    // Composite templates reference records by 1-based index within a sub-group.
    std::map<std::string, int> fGroupStart;
    bool fValid = false;
};

std::string lower(std::string s) {
    for(auto& c : s) c = char(tolower((unsigned char)c));
    return s;
}

void applyInterfaceGreenMask(SDL_Surface* const surf) {
    if(!surf || surf->format->format != SDL_PIXELFORMAT_RGBA32) return;
    SDL_LockSurface(surf);
    auto* const px = static_cast<uint8_t*>(surf->pixels);
    for(int y = 0; y < surf->h; y++) {
        auto* const row = reinterpret_cast<uint32_t*>(px + y * surf->pitch);
        for(int x = 0; x < surf->w; x++) {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
            SDL_GetRGBA(row[x], surf->format, &r, &g, &b, &a);
            if(a == 0) continue;
            if(g >= 180 && r <= 40 && b <= 80 && g > r * 4 && g > b * 3) {
                row[x] = SDL_MapRGBA(surf->format, r, g, b, 0);
            }
        }
    }
    SDL_UnlockSurface(surf);
}

uint16_t rd16(const std::vector<uint8_t>& d, const size_t p) {
    return uint16_t(d[p] | (d[p + 1] << 8));
}
uint32_t rd32(const std::vector<uint8_t>& d, const size_t p) {
    return uint32_t(d[p] | (d[p + 1] << 8) | (d[p + 2] << 16) | (d[p + 3] << 24));
}

bool readWhole(const std::string& path, std::vector<uint8_t>& out) {
    std::ifstream f(path, std::ios::in | std::ios::binary | std::ios::ate);
    if(!f) return false;
    const std::streamoff sz = f.tellg();
    if(sz <= 0) return false;
    out.resize(size_t(sz));
    f.seekg(0);
    f.read(reinterpret_cast<char*>(out.data()), sz);
    return bool(f);
}

// Bitmap record table: starts after the 80-byte header + 600-byte index,
// 200 bytes per record, count at header offset 20. Each record holds a name and
// (at int offset 116, field [3]) the global record index where its sub-group
// of images begins. Composites address records by 1-based index within these.
constexpr int kBitmapStart = 680;
constexpr int kBitmapSize  = 200;

std::shared_ptr<SgFile> parseSg(const std::string& sgName) {
    const auto sg3Path = GameDir::path("DATA/" + sgName + ".sg3");
    const auto p555Path = GameDir::path("DATA/" + sgName + ".555");

    auto file = std::make_shared<SgFile>();

    std::vector<uint8_t> sg;
    if(!readWhole(sg3Path, sg)) {
        return file; // invalid; caller treats as "no SG source"
    }
    if(!readWhole(p555Path, file->fPixels)) {
        printf("SgReader: '%s' present but '%s.555' missing\n",
               sg3Path.c_str(), sgName.c_str());
        return file;
    }

    const uint32_t version = rd32(sg, 4);
    const uint32_t maxRecords = rd32(sg, 12);
    const uint32_t numBitmaps = rd32(sg, 20);
    const int recSz = (version >= 214) ? kRecSz214 : kRecSz213;
    // The compressed flag sits at byte 51 within the record for both 64- and
    // 72-byte record layouts (verified vs Zeus_Interface v213 + SprAmbient v214).
    const int compOff = 51;

    file->fRecords.resize(maxRecords + 1);
    for(uint32_t i = 0; i <= maxRecords; i++) {
        const size_t b = size_t(kRecordStart) + size_t(i) * recSz;
        if(b + size_t(recSz) > sg.size()) break;
        SgRecord& r = file->fRecords[i];
        r.offset = rd32(sg, b + 0);
        r.dataLength = rd32(sg, b + 4);
        r.width = rd16(sg, b + 20);
        r.height = rd16(sg, b + 22);
        r.compressed = sg[b + compOff] != 0;
        r.isometric = sg[b + 50] == 30; // IMAGE_TYPE_ISOMETRIC
        if(version >= 214) { // 72-byte record appends alpha-mask offset/length
            r.alphaOffset = rd32(sg, b + 64);
            r.alphaLength = rd32(sg, b + 68);
        }
    }

    for(uint32_t i = 0; i < numBitmaps; i++) {
        const size_t b = size_t(kBitmapStart) + size_t(i) * kBitmapSize;
        if(b + 132 > sg.size()) break;
        std::string name;
        for(size_t k = 0; k < 65 && sg[b + k]; k++) name += char(sg[b + k]);
        const auto dot = name.find('.');
        if(dot != std::string::npos) name = name.substr(0, dot); // strip .bmp
        const int start = int(rd32(sg, b + 116 + 3 * 4)); // field [3] = first record
        file->fGroupStart[lower(name)] = start;
    }

    file->fValid = true;
    return file;
}

std::map<std::string, std::shared_ptr<SgFile>>& cache() {
    static std::map<std::string, std::shared_ptr<SgFile>> c;
    return c;
}

const std::shared_ptr<SgFile>& openSg(const std::string& sgName) {
    auto it = cache().find(sgName);
    if(it == cache().end()) {
        it = cache().emplace(sgName, parseSg(sgName)).first;
    }
    return it->second;
}

// One textureTemplates/<name> entry: which .sg3, which sub-group, index range.
struct CompositeTemplate {
    std::string fSgFile;     // e.g. "Zeus_Interface"
    std::string fGroup;      // e.g. "Zeus_Interface_New_parts" (bitmap name)
    int fStart = 0;          // 1-based first index within the sub-group
    int fEnd = 0;            // 1-based last index (inclusive)
    int fOffset = 0;         // record-offset calibration (some groups' first image
                             // does not sit at the bitmap start; e.g. +7)
    bool fValid = false;
};

CompositeTemplate readTemplate(const std::string& name) {
    CompositeTemplate t;
    // Recipes are baked into composite-templates.h (generated from the source
    // textureTemplates/*), so no runtime files need shipping.
    const auto& m = compositeTemplates();
    const auto it = m.find(name);
    if(it == m.end()) return t;
    const CompositeTpl& c = it->second;
    t.fSgFile = c.fSgFile;
    t.fGroup = c.fGroup;
    t.fStart = c.fStart;
    t.fEnd = c.fEnd;
    t.fOffset = c.fOffset;
    t.fValid = t.fEnd >= t.fStart && !t.fSgFile.empty() && !t.fGroup.empty();
    return t;
}

// 16-bit 555 -> 8-bit per channel with low-bit fill (matches Augustus to_32_bit).
inline void to888(const uint16_t c, Uint8& r, Uint8& g, Uint8& b) {
    const int ri = (c >> 10) & 0x1f;
    const int gi = (c >> 5) & 0x1f;
    const int bi = c & 0x1f;
    r = Uint8((ri << 3) | (ri >> 2));
    g = Uint8((gi << 3) | (gi >> 2));
    b = Uint8((bi << 3) | (bi >> 2));
}

SDL_Surface* decodeRecord(const SgFile& f, const SgRecord& r) {
    if(r.width == 0 || r.height == 0 || r.dataLength == 0) return nullptr;
    if(size_t(r.offset) + r.dataLength > f.fPixels.size()) return nullptr;

    const auto surf = SDL_CreateRGBSurfaceWithFormat(
                          0, r.width, r.height, 32, SDL_PIXELFORMAT_RGBA32);
    if(!surf) return nullptr;
    SDL_LockSurface(surf);
    Uint32* const px = static_cast<Uint32*>(surf->pixels);
    const int total = r.width * r.height;
    memset(px, 0, size_t(total) * 4); // fully transparent default

    const uint8_t* const buf = f.fPixels.data() + r.offset;
    const uint32_t len = r.dataLength;

    // Shared RLE walk: ctrl 255 -> skip N indices (transparent run); else ctrl
    // units follow, each emitted via emit(index, dataPtr). unitBytes advances the
    // cursor per unit (2 for 16-bit color, 1 for alpha). Returns ending index.
    auto rle = [](const uint8_t* const d, const uint32_t dlen, const int cap,
                  const int unitBytes, int start,
                  const std::function<void(int, const uint8_t*)>& emit) {
        uint32_t pos = 0;
        int i = start;
        while(pos < dlen && i < cap) {
            const uint8_t ctrl = d[pos++];
            if(ctrl == 255) {
                if(pos >= dlen) break;
                i += d[pos++]; // transparent run
            } else {
                for(int k = 0; k < ctrl && pos + unitBytes - 1 < dlen &&
                               i < cap; k++) {
                    emit(i++, d + pos);
                    pos += unitBytes;
                }
            }
        }
        return i;
    };
    auto color16 = [](const uint8_t* const p) {
        return uint16_t(p[0] | (p[1] << 8));
    };

    // No alpha mask -> the Impressions shadow convention applies: pure red is a
    // flat grey shadow. Alpha-mask records (v214 gods) use red as real glow, so
    // skip the transform and let the mask supply transparency.
    const bool shadowMagic = r.alphaLength == 0;
    auto put = [&](const int i, const uint16_t c) {
        if(i < 0 || i >= total) return;
        if(c == kTransparent) { px[i] = 0; return; }
        Uint8 rr, gg, bb;
        to888(c, rr, gg, bb);
        if(shadowMagic && rr >= 250 && gg <= 5 && bb <= 5) {
            // Semi-transparent dark shadow so the ground shows through.
            px[i] = SDL_MapRGBA(surf->format, 0, 0, 0, 96);
            return;
        }
        px[i] = SDL_MapRGBA(surf->format, rr, gg, bb, 255);
    };

    if(r.isometric) {
        // Isometric tile: one or more 58x30 footprint diamonds (uncompressed
        // 16-bit) laid out in a diamond staircase at the bottom of the rect, then
        // an optional compressed "top" for tall objects drawn from y=0.
        // Per-row x-start of the 58-wide, 30-tall diamond (matches Augustus).
        static const int kXStart[30] = {
            28,26,24,22,20,18,16,14,12,10,8,6,4,2,0,
            0,2,4,6,8,10,12,14,16,18,20,22,24,26,28
        };
        constexpr int kFW = 58;
        constexpr int kFH = 30;
        constexpr int kHalf = 15;
        const int tiles = (r.width + 2) / (kFW + 2);
        const int nTiles = tiles > 0 ? tiles : 1;
        const int footH = kFH * nTiles;
        const int yOff = r.height - footH; // footprint sits at the bottom
        uint32_t pos = 0;

        auto footTile = [&](const int xo, const int yo) {
            for(int ty = 0; ty < kFH && pos + 1 < len; ty++) {
                const int xs = kXStart[ty];
                const int py = yOff + yo + ty;
                for(int x = xs; x < kFW - xs && pos + 1 < len; x++) {
                    const uint16_t c = uint16_t(buf[pos] | (buf[pos + 1] << 8));
                    pos += 2;
                    const int px2 = xo + x;
                    if(px2 >= 0 && px2 < r.width && py >= 0 && py < r.height) {
                        put(py * r.width + px2, c);
                    }
                }
            }
        };

        // Diamond staircase (Augustus convert_isometric_footprint).
        const int xStart = (nTiles - 1) * 30;
        for(int i = 0; i < nTiles; i++) {
            int x = -30 * i + xStart;
            const int y = kHalf * i;
            for(int j = 0; j <= i; j++) { footTile(x, y); x += 60; }
        }
        for(int i = nTiles - 2; i >= 0; i--) {
            int x = -30 * i + xStart;
            const int y = kHalf * (nTiles * 2 - i - 2);
            for(int j = 0; j <= i; j++) { footTile(x, y); x += 60; }
        }

        // Compressed top (if any) fills the upper region from index 0.
        int i = 0;
        const int topTotal = (r.height - footH / 2) * r.width;
        while(pos < len && i < topTotal && i < total) {
            const uint8_t ctrl = buf[pos++];
            if(ctrl == 255) {
                if(pos >= len) break;
                i += buf[pos++];
            } else {
                for(int k = 0; k < ctrl && pos + 1 < len && i < total; k++) {
                    const uint16_t c = uint16_t(buf[pos] | (buf[pos + 1] << 8));
                    pos += 2;
                    put(i++, c);
                }
            }
        }
    } else if(r.compressed) {
        rle(buf, len, total, 2, 0,
            [&](const int i, const uint8_t* const p) { put(i, color16(p)); });
    } else {
        uint32_t pos = 0;
        for(int i = 0; i < total && pos + 1 < len; i++) {
            const uint16_t c = uint16_t(buf[pos] | (buf[pos + 1] << 8));
            pos += 2;
            put(i, c);
        }
    }

    // v214 alpha mask: a second RLE image stored right after the RGB data.
    // ctrl 255 -> skip N transparent pixels; else N bytes, each 5-bit alpha.
    // Scales 5 bits to 8 (bvschaik citybuilding-tools SgImage::setAlphaPixel).
    if(r.alphaLength > 0) {
        const size_t ab = size_t(r.offset) + r.dataLength;
        if(ab + r.alphaLength <= f.fPixels.size()) {
            const uint8_t* const abuf = f.fPixels.data() + ab;
            rle(abuf, r.alphaLength, total, 1, 0,
                [&](const int i, const uint8_t* const p) {
                    const uint8_t c = *p;
                    const Uint8 a = Uint8(((c & 0x1f) << 3) | ((c & 0x1c) >> 2));
                    Uint8 rr, gg, bb, oa;
                    SDL_GetRGBA(px[i], surf->format, &rr, &gg, &bb, &oa);
                    px[i] = SDL_MapRGBA(surf->format, rr, gg, bb, a);
                });
        }
    }

    SDL_UnlockSurface(surf);
    return surf;
}

SDL_Surface* scaleSurface(SDL_Surface* const src, const int tileH) {
    if(tileH == 30 || tileH <= 0) return src; // native tier
    const double ratio = double(tileH) / 30.0;
    const int w = int(src->w * ratio + 0.5);
    const int h = int(src->h * ratio + 0.5);
    if(w <= 0 || h <= 0) return src;
    const auto dst = SDL_CreateRGBSurfaceWithFormat(
                         0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if(!dst) return src;
    SDL_BlitScaled(src, nullptr, dst, nullptr); // nearest-ish, preserves alpha
    SDL_FreeSurface(src);
    return dst;
}

// Parse "30/SprAmbient/SprAmbient_00009.png" -> tileH, group, 1-based index.
bool parsePath(const std::string& path, int& tileH,
               std::string& group, int& index) {
    const auto s1 = path.find('/');
    if(s1 == std::string::npos) return false;
    tileH = atoi(path.substr(0, s1).c_str());
    const auto s2 = path.find('/', s1 + 1);
    if(s2 == std::string::npos) return false;
    group = path.substr(s1 + 1, s2 - s1 - 1);
    const std::string file = path.substr(s2 + 1);
    const auto us = file.rfind('_');
    const auto dot = file.rfind('.');
    if(us == std::string::npos || dot == std::string::npos || dot <= us) return false;
    index = atoi(file.substr(us + 1, dot - us - 1).c_str());
    return index > 0 && !group.empty();
}

} // namespace

SDL_Surface* SgReader::load(const std::string& path) {
    // Full-screen art (load/map/defeat screens) ships in DATA as standalone
    // image files under the eZeus "Zeus_Data_Images" pseudo-group, e.g.
    // "45/Zeus_Data_Images/Zeus_Load1.jpg" -> DATA/Zeus_Load1.jpg. Load directly.
    {
        const std::string marker = "/Zeus_Data_Images/";
        const auto m = path.find(marker);
        if(m != std::string::npos) {
            const std::string file = path.substr(m + marker.size());
            const std::string full = GameDir::path("DATA/" + file);
            SDL_Surface* const loaded = IMG_Load(full.c_str());
            if(!loaded) {
                printf("SG miss '%s': DATA image '%s' not loadable (%s)\n",
                       path.c_str(), full.c_str(), IMG_GetError());
                return nullptr;
            }
            const auto rgba = SDL_ConvertSurfaceFormat(
                                  loaded, SDL_PIXELFORMAT_RGBA32, 0);
            SDL_FreeSurface(loaded);
            return rgba; // no scaling: these are absolute-size screens
        }
    }

    int tileH = 30;
    std::string group;
    int index = 0;
    if(!parsePath(path, tileH, group, index)) {
        // 2-segment "zoom/name_idx" paths are eZeus composites - handled by the
        // loadComposite path in SpriteLoader, not here. Stay silent for those;
        // only warn on genuinely malformed paths.
        const auto s1 = path.find('/');
        const bool composite = s1 != std::string::npos &&
                               path.find('/', s1 + 1) == std::string::npos;
        if(!composite) {
            printf("SG miss '%s': unparseable path\n", path.c_str());
        }
        return nullptr;
    }

    const auto& f = *openSg(group);
    if(!f.fValid) {
        printf("SG miss '%s': no DATA/%s.sg3+.555 (eZeus composite, no 1:1 record)\n",
               path.c_str(), group.c_str());
        return nullptr;
    }
    if(index < 0 || size_t(index) >= f.fRecords.size()) {
        printf("SG miss '%s': index %d out of range (%zu records in %s)\n",
               path.c_str(), index, f.fRecords.size(), group.c_str());
        return nullptr;
    }

    const SgRecord& rec = f.fRecords[index];
    if(rec.width == 0 || rec.height == 0 || rec.dataLength == 0) {
        printf("SG miss '%s': record %d is empty (%dx%d dl=%u)\n",
               path.c_str(), index, rec.width, rec.height, rec.dataLength);
        return nullptr;
    }

    SDL_Surface* surf = decodeRecord(f, rec);
    if(!surf) {
        printf("SG miss '%s': decode failed (off=%u dl=%u %dx%d comp=%d)\n",
               path.c_str(), rec.offset, rec.dataLength,
               rec.width, rec.height, rec.compressed ? 1 : 0);
        return nullptr;
    }
    return scaleSurface(surf, tileH);
}

SDL_Surface* SgReader::loadComposite(const std::string& name,
                                     const int tileH,
                                     const int texId,
                                     const std::vector<eSpriteData>& sds) {
    const CompositeTemplate t = readTemplate(name);
    if(!t.fValid) {
        printf("SG composite '%s' tex%d: no template textureTemplates/%s\n",
               name.c_str(), texId, name.c_str());
        return nullptr;
    }

    const auto& f = *openSg(t.fSgFile);
    if(!f.fValid) {
        printf("SG composite '%s' tex%d: cannot open DATA/%s.sg3+.555\n",
               name.c_str(), texId, t.fSgFile.c_str());
        return nullptr;
    }

    // Sub-group global offset from the bitmap table. groupStart is the 1-based
    // global record index of the sub-group's first image, so
    // global = groupStart + (subIndex - 1). When the group prefix is the file's
    // own sprite name (SprMain, SprAmbient, creature files) there is no sub-group
    // - the template index is already the global record index, i.e. start == 1.
    int groupStart = 1;
    bool subGroup = false; // true: real bitmap sub-group (non-empty numbering)
    const auto gs = f.fGroupStart.find(lower(t.fGroup));
    if(gs != f.fGroupStart.end() && gs->second > 0) {
        groupStart = gs->second;
        subGroup = true;
    }

    // Strip size = bounding box of the spriteData entries on this texId.
    int stripW = 0;
    int stripH = 0;
    for(const auto& sd : sds) {
        if(sd.fTexId != texId) continue;
        stripW = std::max(stripW, sd.fX + sd.fW);
        stripH = std::max(stripH, sd.fY + sd.fH);
    }
    if(stripW <= 0 || stripH <= 0) return nullptr;

    const auto strip = SDL_CreateRGBSurfaceWithFormat(
                           0, stripW, stripH, 32, SDL_PIXELFORMAT_RGBA32);
    if(!strip) return nullptr;
    SDL_FillRect(strip, nullptr, 0); // transparent

    // For real bitmap sub-groups, eZeus's sub-index is a 1-based counter over the
    // group's NON-EMPTY records - the .sg3 scatters empty records inside a group
    // that eZeus's numbering skips. So sub-index N = the Nth non-empty record at/
    // after groupStart. For whole-file passthrough groups (SprMain, SprAmbient,
    // creatures - no bitmap sub-group), the index is the direct global record.
    const size_t recCount = f.fRecords.size();
    std::vector<int> nonEmpty; // nonEmpty[n] = global record for sub-index n+1
    if(subGroup) {
        nonEmpty.reserve(t.fEnd + 8);
        for(size_t g = size_t(groupStart); g < recCount; g++) {
            const SgRecord& rr = f.fRecords[g];
            if(rr.width == 0 || rr.height == 0 || rr.dataLength == 0) continue;
            nonEmpty.push_back(int(g));
            if(int(nonEmpty.size()) >= t.fEnd + 1) break;
        }
    }

    for(size_t k = 0; k < sds.size(); k++) {
        const eSpriteData& sd = sds[k];
        if(sd.fTexId != texId) continue;
        if(sd.fW <= 0 || sd.fH <= 0) continue;
        const int subIndex = t.fStart + int(k); // 1-based within sub-group
        int global;
        if(subGroup) {
            const int ni = subIndex - 1 + t.fOffset;
            if(ni < 0 || size_t(ni) >= nonEmpty.size()) continue;
            global = nonEmpty[ni];
        } else {
            global = subIndex + t.fOffset; // direct global record index
            if(global < 0 || size_t(global) >= recCount) continue;
        }
        const SgRecord& rec = f.fRecords[global];
        SDL_Surface* const rs = decodeRecord(f, rec);
        if(!rs) continue;
        SDL_Rect dst{sd.fX, sd.fY, sd.fW, sd.fH}; // rect already in target zoom
        SDL_SetSurfaceBlendMode(rs, SDL_BLENDMODE_NONE);
        SDL_BlitScaled(rs, nullptr, strip, &dst); // scales native record to rect
        SDL_FreeSurface(rs);
    }

    (void)tileH; // rects are already zoom-specific; record is scaled into them
    if(name == "interfaceNewParts") {
        applyInterfaceGreenMask(strip);
    }
    return strip;
}
