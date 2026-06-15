#ifndef EPYRAMIDTILE_H
#define EPYRAMIDTILE_H

#include "epyramidelement.h"

class ePyramidTile : public ePyramidElement {
public:
    ePyramidTile(const std::vector<eSanctCost>& cost,
                 GameBoard& board,
                 const int elevation,
                 const int type,
                 const eCityId cid);
    ePyramidTile(ePyramid* const pyramid,
                 GameBoard& board,
                 const int elevation,
                 const int type,
                 const eCityId cid);

    stdsptr<Texture> getTexture(const eTileSize size) const override;

    int type() const { return mType; }
private:
    const int mType;
};

#endif // EPYRAMIDTILE_H
