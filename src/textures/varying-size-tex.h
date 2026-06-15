#ifndef VARYING_SIZE_TEX_H
#define VARYING_SIZE_TEX_H

#include "engine/etile.h"
#include "texture-collection.h"

namespace VaryingSizeTex {
    using eChecker = std::function<bool(eTile*)>;
    void get(eTile* const tile,
             const eChecker& checker,
             int& drawDim,
             const eWorldDirection dir);

    eTile* hiddenByNeighbour(
        eTile* const tile,
        const eWorldDirection dir,
        int& dx, int& dy);
    using eVaryingFunc = std::function<void(eTile* const tile,
                                            int& drawDim,
                                            const eWorldDirection dir)>;
    std::shared_ptr<Texture> getVaryingTexture(
            const eVaryingFunc& func,
            eTile* const tile,
            const TextureCollection& small,
            const TextureCollection& large,
            const TextureCollection& huge,
            int& drawDim,
            const eWorldDirection dir);
};

#endif // VARYING_SIZE_TEX_H
