#ifndef EPYRAMIDBUILDINGPART_H
#define EPYRAMIDBUILDINGPART_H

#include "epyramidelement.h"

class ePyramidBuildingPart : public ePyramidElement {
public:
    ePyramidBuildingPart(const std::vector<eSanctCost>& cost,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid);
    ePyramidBuildingPart(ePyramid* const pyramid,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize) const override {
        return nullptr;
    }
};

#endif // EPYRAMIDBUILDINGPART_H
