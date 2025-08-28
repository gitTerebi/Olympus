#ifndef EVARYINGSIZETEX_H
#define EVARYINGSIZETEX_H

#include "engine/etile.h"
#include "etexturecollection.h"

namespace eVaryingSizeTex {
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
    std::shared_ptr<eTexture> getVaryingTexture(
            const eVaryingFunc& func,
            eTile* const tile,
            const eTextureCollection& small,
            const eTextureCollection& large,
            const eTextureCollection& huge,
            int& drawDim,
            const eWorldDirection dir);
};

#endif // EVARYINGSIZETEX_H
