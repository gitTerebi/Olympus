#ifndef SPRITE_LOADER_H
#define SPRITE_LOADER_H

#include "texture-collection.h"
#include "spriteData/espritedata.h"
#include "game-dir.h"
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
                     TextureCollection& coll,
                     const int dy);
    void loadArrowSkipFlipped(const int doff,
                              const int min, const int max,
                              TextureCollection& coll);
    void loadSkipFlipped(const int doff,
                         const int min, const int max,
                         TextureCollection& coll);
    void loadSkipFlipped(const int doff,
                         const int min, const int max,
                         std::vector<TextureCollection>& colls);
    void loadHorseSkipFlipped(const int doff,
                              const int min, const int max,
                              std::vector<TextureCollection>& colls);
    void loadBoatSkipFlipped(const int doff,
                             const int min, const int max,
                             std::vector<TextureCollection>& colls);

    const std::shared_ptr<Texture>& load(
            const int doff, const int i,
            TextureCollection& coll);

    std::shared_ptr<Texture> load(
            const int doff, const int i);
private:
    void loadTex(const int i);
    void buildSpriteAtlas();

    const std::shared_ptr<Texture>& getTex(const int i);;

    const std::string mSize;
    const std::string mName;
    const std::vector<eSpriteData> mSds;
    const eOffsets* const mOffs;
    SDL_Renderer* const mRenderer;
    std::map<int, std::shared_ptr<Texture>> mTexs;
};

#endif // SPRITE_LOADER_H
