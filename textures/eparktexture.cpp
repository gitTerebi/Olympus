#include "eparktexture.h"

#include "evaryingsizetex.h"
#include "buildings/ebuilding.h"

void eParkTexture::get(eTile* const tile,
                       int& drawDim,
                       const eWorldDirection dir) {
    const auto checker = [](eTile* const t) {
        return t->underBuildingType() == eBuildingType::park;
    };
    eVaryingSizeTex::get(tile, checker, drawDim, dir);
}
