#include "sprite-loader.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "sg-reader.h"

SpriteLoader::~SpriteLoader() {
    buildSpriteAtlas();
}

void SpriteLoader::buildSpriteAtlas() {
    if(mTexs.size() < 2) return;

    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(mRenderer, &info) != 0) return;
    int maxSize = std::min(info.max_texture_width, info.max_texture_height);
    if(maxSize <= 0) maxSize = 4096;
    maxSize = std::min(maxSize, 8192);

    struct Entry {
        std::shared_ptr<Texture> fTex;
        SDL_Rect fDst;
    };
    std::vector<Entry> entries;
    entries.reserve(mTexs.size());

    int x = 0;
    int y = 0;
    int rowH = 0;
    int atlasW = 0;
    int atlasH = 0;
    int64_t area = 0;
    for(const auto& pair : mTexs) {
        const auto& tex = pair.second;
        if(!tex || !tex->tex()) return;
        const int w = tex->width();
        const int h = tex->height();
        if(w <= 0 || h <= 0 || w > maxSize || h > maxSize) return;
        if(x + w > maxSize) {
            y += rowH;
            x = 0;
            rowH = 0;
        }
        if(y + h > maxSize) return;
        entries.push_back({tex, SDL_Rect{x, y, w, h}});
        x += w;
        rowH = std::max(rowH, h);
        atlasW = std::max(atlasW, x);
        atlasH = std::max(atlasH, y + rowH);
        area += int64_t(w) * h;
    }
    if(entries.size() < 2 || atlasW <= 0 || atlasH <= 0) return;
    if(area > 8192ll * 8192ll) return;

    const auto atlas = std::make_shared<Texture>();
    if(!atlas->create(mRenderer, atlasW, atlasH)) return;

    const auto prevTarget = SDL_GetRenderTarget(mRenderer);
    SDL_BlendMode prevBlendMode;
    SDL_GetRenderDrawBlendMode(mRenderer, &prevBlendMode);
    atlas->setAsRenderTarget(mRenderer);
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mRenderer);
    SDL_SetRenderDrawBlendMode(mRenderer, prevBlendMode);
    for(const auto& e : entries) {
        SDL_RenderCopy(mRenderer, e.fTex->tex(), nullptr, &e.fDst);
    }
    SDL_SetRenderTarget(mRenderer, prevTarget);
    SDL_SetRenderDrawBlendMode(mRenderer, prevBlendMode);

    for(const auto& e : entries) {
        e.fTex->setParentTexture(e.fDst, atlas);
    }
}

void SpriteLoader::loadTrailer(const int doff,
                                const int min, const int max,
                                TextureCollection& coll, const int dy) {
    loadSkipFlipped(doff, min, max, coll);
    for(int i = 0; i < max - min; i++) {
        const auto& tex = coll.getTexture(i);
        tex->setOffset(tex->offsetX(), tex->offsetY() + dy);
    }
}

void SpriteLoader::loadArrowSkipFlipped(const int doff,
                                         const int min, const int max,
                                         TextureCollection& coll) {
    for(int i = min; i < max; i++) {
        if(i - min > 15 && i - min < 31) {
            auto& tex = coll.addTexture();
            const auto& flipTex = coll.getTexture(min + 30 - i);
            tex->setFlipTex(flipTex);
            if(mOffs) {
                const auto& offset = (*mOffs)[i - 1];
                tex->setOffset(offset.first, offset.second);
            }
        } else {
            load(doff, i, coll);
        }
    }
}

void SpriteLoader::loadSkipFlipped(const int doff,
                                    const int min, const int max,
                                    TextureCollection& coll) {
    for(int i = min; i < max;) {
        for(int j = 0; j < 8; j++, i++) {
            if(j > 3 && j < 7) {
                auto& tex = coll.addTexture();
                const auto& flipTex = coll.getTexture(6 - j);
                tex->setFlipTex(flipTex);
                if(mOffs) {
                    const auto& offset = (*mOffs)[i - 1];
                    tex->setOffset(offset.first, offset.second);
                }
            } else {
                load(doff, i, coll);
            }
        }
    }
}

void SpriteLoader::loadSkipFlipped(const int doff,
                                    const int min, const int max,
                                    std::vector<TextureCollection>& colls) {
    for(int j = 0; j < 8; j++) {
        colls.emplace_back(mRenderer);
    }
    int k = 0;
    for(int i = min; i < max;) {
        for(int j = 0; j < 8; j++, i++) {
            auto& coll = colls[j];
            if(j > 3 && j < 7) {
                const auto& flipTex = colls[6 - j].getTexture(k);
                auto& tex = coll.addTexture();
                tex->setFlipTex(flipTex);
                if(mOffs) {
                    const auto& offset = (*mOffs)[i - 1];
                    tex->setOffset(offset.first, offset.second);
                }
            } else {
                load(doff, i, coll);
            }
        }
        k++;
    }
}

void SpriteLoader::loadHorseSkipFlipped(const int doff,
                                         const int min, const int max,
                                         std::vector<TextureCollection> &colls) {
    for(int j = 0; j < 16; j++) {
        colls.emplace_back(mRenderer);
    }
    int k = 0;
    for(int i = min; i < max;) {
        for(int j = 0; j < 16; j++, i++) {
            auto& coll = colls[j];
            if(j > 7 && j < 15) {
                const auto& flipTex = colls[14 - j].getTexture(k);
                auto& tex = coll.addTexture();
                tex->setFlipTex(flipTex);
                if(mOffs) {
                    const auto& offset = (*mOffs)[i - 1];
                    tex->setOffset(offset.first, offset.second);
                }
            } else {
                load(doff, i, coll);
            }
        }
        k++;
    }
}

void SpriteLoader::loadBoatSkipFlipped(const int doff,
                                        const int min, const int max,
                                        std::vector<TextureCollection>& colls) {
    for(int j = 0; j < 8; j++) {
        colls.emplace_back(mRenderer);
    }
    int k = 0;
    for(int i = min; i < max;) {
        for(int j = 0; j < 8; j++, i += 2) {
            auto& coll = colls[j];
            if(j > 3 && j < 7) {
                const auto& flipTex = colls[6 - j].getTexture(k);
                auto& tex = coll.addTexture();
                tex->setFlipTex(flipTex);
                if(mOffs) {
                    const auto& offset = (*mOffs)[i - 1];
                    tex->setOffset(offset.first, offset.second);
                }
            } else {
                load(doff, i, coll);
            }
        }
        k++;
    }
}

const std::shared_ptr<Texture>& SpriteLoader::load(
        const int doff, const int i, TextureCollection& coll) {
    const auto& sd = mSds[i - doff];
    const int tid = sd.fTexId;
    const auto t = tid == -1 ? nullptr : getTex(tid);
    const SDL_Rect rect{sd.fX, sd.fY, sd.fW, sd.fH};
    const auto& tex = coll.addTexture();
    tex->setParentTexture(rect, t);
    if(mOffs) {
        const auto& off = (*mOffs)[i - 1];
        tex->setOffset(off.first, off.second);
    }
    return tex;
}

std::shared_ptr<Texture> SpriteLoader::load(
        const int doff, const int i) {
    const auto& sd = mSds[i - doff];
    const int tid = sd.fTexId;
    const auto t = getTex(tid);
    std::shared_ptr<Texture> tex;
    if(mSds.size() == 1) {
        tex = t;
    } else {
        const SDL_Rect rect{sd.fX, sd.fY, sd.fW, sd.fH};
        tex = std::make_shared<Texture>();
        tex->setParentTexture(rect, t);
    }
    if(mOffs) {
        const auto& off = (*mOffs)[i - 1];
        tex->setOffset(off.first, off.second);
    }
    return tex;
}

void SpriteLoader::loadTex(const int i) {
    const bool binary = true;
    std::shared_ptr<Texture> tex;
    if(binary) {
        const auto path = mSize + "/" + mName + "_" + std::to_string(i) + ".png";
        tex = BinaryImageLoader::load(mRenderer, path);
        if(!tex) {
            // Composite strip: build it live from the player's .555 via the
            // textureTemplates/<name> recipe + this zoom's spriteData rects.
            const auto surf = SgReader::loadComposite(
                                  mName, atoi(mSize.c_str()), i, mSds);
            if(surf) {
                tex = std::make_shared<Texture>();
                tex->load(mRenderer, surf); // takes ownership of surf
            }
        }
    } else {
        tex = std::make_shared<Texture>();
        const std::string dir = GameDir::texturesDir() + mSize + "/";
        const auto path = dir + mName + "_" + std::to_string(i) + ".png";
        tex->load(mRenderer, path);
    }
    mTexs[i] = tex;
}

const std::shared_ptr<Texture>& SpriteLoader::getTex(const int i) {
    if(mTexs.find(i) == mTexs.end()) {
        loadTex(i);
    }
    return mTexs[i];
}
