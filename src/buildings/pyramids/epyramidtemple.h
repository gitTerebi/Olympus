#ifndef EPYRAMIDTEMPLE_H
#define EPYRAMIDTEMPLE_H

#include "epyramidelement.h"

class ePyramidTemple : public ePyramidElement {
public:
    ePyramidTemple(const std::vector<eSanctCost>& cost,
                   GameBoard& board,
                   const int elevation,
                   const eCityId cid);
    ePyramidTemple(ePyramid* const pyramid,
                   GameBoard& board,
                   const int elevation,
                   const eCityId cid);

    stdsptr<Texture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDTEMPLE_H
