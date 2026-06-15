#ifndef EPYRAMIDALTAR_H
#define EPYRAMIDALTAR_H

#include "epyramidelement.h"

class ePyramidAltar : public ePyramidElement {
public:
    ePyramidAltar(const std::vector<eSanctCost>& cost,
                  GameBoard& board,
                  const int elevation,
                  const eCityId cid);
    ePyramidAltar(ePyramid* const pyramid,
                  GameBoard& board,
                  const int elevation,
                  const eCityId cid);

    stdsptr<Texture> getTexture(const eTileSize size) const override;
};

#endif // EPYRAMIDALTAR_H
