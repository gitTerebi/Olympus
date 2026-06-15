#ifndef SPRITE_LOADER_H
#define SPRITE_LOADER_H

#include "etexturecollection.h"
#include "spriteData/espritedata.h"
#include "egamedir.h"
#include "binary-image-loader.h"

using eOffset = std::pair<int, int>;
using eOffsets = std::vector<eOffset>;

class SpriteLoader {
public:
    SpriteLoader(const int size,
                  const std::string& name,
                  const std::vector<eSpriteData>& sds,
                  const eOffsets* const offs,
                  SDL_Renderer* const r) :
        mSize(std::to_string(size)), mName(name),
        mSds(sds), mOffs(offs), mRenderer(r) {}
    ~SpriteLoader();

    void loadTrailer(const int doff,
                     const int min, const int max,
                     eTextureCollection& coll,
                     const int dy);
    void loadArrowSkipFlipped(const int doff,
                              const int min, const int max,
                              eTextureCollection& coll);
    void loadSkipFlipped(const int doff,
                         const int min, const int max,
                         eTextureCollection& coll);
    void loadSkipFlipped(const int doff,
                         const int min, const int max,
                         std::vector<eTextureCollection>& colls);
    void loadHorseSkipFlipped(const int doff,
                              const int min, const int max,
                              std::vector<eTextureCollection>& colls);
    void loadBoatSkipFlipped(const int doff,
                             const int min, const int max,
                             std::vector<eTextureCollection>& colls);

    const std::shared_ptr<eTexture>& load(
            const int doff, const int i,
            eTextureCollection& coll);

    std::shared_ptr<eTexture> load(
            const int doff, const int i);
private:
    void loadTex(const int i);
    void buildSpriteAtlas();

    const std::shared_ptr<eTexture>& getTex(const int i);;

    const std::string mSize;
    const std::string mName;
    const std::vector<eSpriteData> mSds;
    const eOffsets* const mOffs;
    SDL_Renderer* const mRenderer;
    std::map<int, std::shared_ptr<eTexture>> mTexs;
};

inline const std::vector<eSpriteData>& spriteData(
        const int size,
        const std::vector<eSpriteData>& s15,
        const std::vector<eSpriteData>& s30,
        const std::vector<eSpriteData>& s45,
        const std::vector<eSpriteData>& s60) {
    if(size == 15) return s15;
    if(size == 30) return s30;
    if(size == 45) return s45;
    if(size == 60) return s60;
    return s30;
}

#endif // SPRITE_LOADER_H
