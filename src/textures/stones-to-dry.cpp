#include "stones-to-dry.h"

#include "varying-size-tex.h"

void StonesToDry::get(eTile* const tile,
                       int& drawDim,
                       const eWorldDirection dir) {
    const auto terr = tile->terrain();
    const auto checker = [terr](eTile* const t) {
        return t->terrain() == terr;
    };
    VaryingSizeTex::get(tile, checker, drawDim, dir);
}
