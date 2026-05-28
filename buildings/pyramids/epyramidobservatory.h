#ifndef EPYRAMIDOBSERVATORY_H
#define EPYRAMIDOBSERVATORY_H

#include "epyramidelement.h"

class ePyramidObservatory : public ePyramidElement {
public:
    ePyramidObservatory(const std::vector<eSanctCost>& cost,
                        GameBoard& board,
                        const int elevation,
                        const eCityId cid);
    ePyramidObservatory(ePyramid* const pyramid,
                        GameBoard& board,
                        const int elevation,
                        const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDOBSERVATORY_H
