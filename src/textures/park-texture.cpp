#include "park-texture.h"

#include "varying-size-tex.h"
#include "buildings/ebuilding.h"

void ParkTexture::get(eTile* const tile,
                       int& drawDim,
                       const eWorldDirection dir) {
    const auto checker = [](eTile* const t) {
        return t->underBuildingType() == eBuildingType::park;
    };
    VaryingSizeTex::get(tile, checker, drawDim, dir);
}
