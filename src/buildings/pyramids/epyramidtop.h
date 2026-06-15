#ifndef EPYRAMIDTOP_H
#define EPYRAMIDTOP_H

#include "epyramidelement.h"

class ePyramidTop : public ePyramidElement {
public:
    ePyramidTop(const std::vector<eSanctCost>& cost,
                GameBoard& board,
                const int elevation,
                const eCityId cid);
    ePyramidTop(ePyramid* const pyramid,
                GameBoard& board,
                const int elevation,
                const eCityId cid);

    stdsptr<Texture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDTOP_H
