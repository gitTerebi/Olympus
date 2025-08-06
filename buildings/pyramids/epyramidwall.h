#ifndef EPYRAMIDWALL_H
#define EPYRAMIDWALL_H

#include "epyramidelement.h"

class ePyramidWall : public ePyramidElement {
public:
    ePyramidWall(const std::vector<eSanctCost>& cost,
                 eGameBoard& board,
                 const eOrientation o,
                 const int elevation,
                 const int special,
                 const eCityId cid);
    ePyramidWall(ePyramid* const pyramid,
                 eGameBoard& board,
                 const eOrientation o,
                 const int elevation,
                 const int special,
                 const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;

    eOrientation orientation() const { return mO; }
    int special() const { return mSpecial; }
private:
    const eOrientation mO;
    const int mSpecial;
};

#endif // EPYRAMIDWALL_H
