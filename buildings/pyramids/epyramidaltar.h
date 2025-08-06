#ifndef EPYRAMIDALTAR_H
#define EPYRAMIDALTAR_H

#include "epyramidelement.h"

class ePyramidAltar : public ePyramidElement {
public:
    ePyramidAltar(const std::vector<eSanctCost>& cost,
                  eGameBoard& board,
                  const int elevation,
                  const eCityId cid);
    ePyramidAltar(ePyramid* const pyramid,
                  eGameBoard& board,
                  const int elevation,
                  const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDALTAR_H
