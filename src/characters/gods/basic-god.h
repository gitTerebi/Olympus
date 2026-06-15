#ifndef EBASICGOD_H
#define EBASICGOD_H

#include "god.h"

#include "textures/god-textures.h"

class BasicGod : public God {
public:
    using eGodTexs = BasicGodTextures GodTextures::*;
    BasicGod(GameBoard& board,
              const eGodTexs godTexs,
              const GodType gt);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
private:
    const eGodTexs mGodTexs;
};

#endif // EBASICGOD_H
