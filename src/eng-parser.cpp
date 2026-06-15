#include "eng-parser.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <fstream>

// Format reference: bvschaik/citybuilding-tools (engconverter).
// All ints little-endian; host is x86 LE so direct reads work.

namespace {

bool readFile(const std::string& filePath, std::vector<char>& out) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if(!file.good()) {
        printf("File missing %s\n", filePath.c_str());
        return false;
    }
    const std::streamsize size = file.tellg();
    if(size <= 0) return false;
    file.seekg(0, std::ios::beg);
    out.resize(static_cast<size_t>(size));
    if(!file.read(out.data(), size)) return false;
    return true;
}

int32_t readInt(const std::vector<char>& d, const size_t off) {
    int32_t v;
    memcpy(&v, &d[off], sizeof(v));
    return v;
}

// Reads a null-terminated string from the blob at the given offset.
// Stops at the blob end if no terminator is present.
std::string readBlobStr(const char* blob, const size_t blobSize,
                        const size_t offset) {
    if(offset >= blobSize) return std::string();
    size_t end = offset;
    while(end < blobSize && blob[end] != '\0') end++;
    return std::string(blob + offset, end - offset);
}

}

bool EngParser::sParse(eTextStrings& strings,
                       const std::string& filePath) {
    std::vector<char> d;
    if(!readFile(filePath, d)) return false;

    // header: 16-byte name, then 12 skipped bytes (recomputed counts)
    const size_t kIndexStart = 28;
    const int kMaxEntries = 1000;
    const size_t kBlobStart = kIndexStart + kMaxEntries * 8;
    if(d.size() < kBlobStart) return false;

    const char* blob = d.data() + kBlobStart;
    const size_t blobSize = d.size() - kBlobStart;

    // index entry = (int32 blob offset, int32 string count); count 0 = absent
    for(int gid = 0; gid < kMaxEntries; gid++) {
        const size_t e = kIndexStart + gid * 8;
        const int32_t offset = readInt(d, e);
        const int32_t count = readInt(d, e + 4);
        if(count <= 0) continue;

        auto& group = strings[gid];
        size_t o = static_cast<size_t>(offset);
        for(int s = 0; s < count; s++) {
            // skip leading zero padding (GOG hack bytes)
            while(o < blobSize && blob[o] == '\0') o++;
            if(o >= blobSize) break;
            const std::string str = readBlobStr(blob, blobSize, o);
            group[s] = str;
            o += str.size() + 1; // step past the trailing \0
        }
    }
    return true;
}

bool EngParser::sParse(eMMStrings& strings,
                       const std::string& filePath) {
    std::vector<char> d;
    if(!readFile(filePath, d)) return false;

    // header: 16-byte name, then int32 total, int32 used
    const size_t kHeader = 24;
    if(d.size() < kHeader) return false;
    const int32_t total = readInt(d, 16);
    if(total < 0) return false;

    const int kRecordSize = 80;
    const size_t kBlobStart = kHeader + static_cast<size_t>(total) * kRecordSize;
    if(d.size() < kBlobStart) return false;

    const char* blob = d.data() + kBlobStart;
    const size_t blobSize = d.size() - kBlobStart;

    // record tail (within the 80-byte record):
    //   +68 title offset, +72 subtitle offset, +76 content offset
    // offset 0 means "no string" (blob starts with zero padding)
    for(int id = 0; id < total; id++) {
        const size_t rec = kHeader + static_cast<size_t>(id) * kRecordSize;
        const int32_t titleOff = readInt(d, rec + 68);
        const int32_t subtitleOff = readInt(d, rec + 72);
        const int32_t contentOff = readInt(d, rec + 76);
        if(titleOff == 0 && subtitleOff == 0 && contentOff == 0) continue;

        eMM& mm = strings[id];
        if(titleOff > 0) mm.fTitle = readBlobStr(blob, blobSize, titleOff);
        if(subtitleOff > 0) mm.fSubtitle = readBlobStr(blob, blobSize, subtitleOff);
        if(contentOff > 0) mm.fContent = readBlobStr(blob, blobSize, contentOff);
    }
    return true;
}
