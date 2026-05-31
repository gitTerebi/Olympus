#include "estonestodry.h"

#include "evaryingsizetex.h"

void eStonesToDry::get(eTile* const tile,
                       int& drawDim,
                       const eWorldDirection dir) {
    const auto terr = tile->terrain();
    const auto checker = [terr](eTile* const t) {
        return t->terrain() == terr;
    };
    eVaryingSizeTex::get(tile, checker, drawDim, dir);
}
