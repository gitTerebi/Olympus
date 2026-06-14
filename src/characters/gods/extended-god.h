#ifndef EEXTENDEDGOD_H
#define EEXTENDEDGOD_H

#include "god.h"

#include "textures/egodtextures.h"

class ExtendedGod : public God {
public:
    using eGodTexs = eExtendedGodTextures eGodTextures::*;
    ExtendedGod(GameBoard& board,
                 const eGodTexs godTexs,
                 const GodType gt);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    const eGodTexs mGodTexs;
};

#endif // EEXTENDEDGOD_H
