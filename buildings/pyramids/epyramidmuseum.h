#ifndef EPYRAMIDMUSEUM_H
#define EPYRAMIDMUSEUM_H

#include "epyramidelement.h"

class ePyramidMuseum : public ePyramidElement {
public:
    ePyramidMuseum(const std::vector<eSanctCost>& cost,
                   GameBoard& board,
                   const int elevation,
                   const eCityId cid);
    ePyramidMuseum(ePyramid* const pyramid,
                   GameBoard& board,
                   const int elevation,
                   const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDMUSEUM_H
