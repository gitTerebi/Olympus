#ifndef EPYRAMIDSTATUE_H
#define EPYRAMIDSTATUE_H

#include "epyramidelement.h"

enum class GodType;

class ePyramidStatue : public ePyramidElement {
public:
    ePyramidStatue(const std::vector<eSanctCost>& cost,
                   GameBoard& board,
                   const int elevation,
                   const GodType type,
                   const int id,
                   const eCityId cid);
    ePyramidStatue(ePyramid* const pyramid,
                   GameBoard& board,
                   const int elevation,
                   const GodType type,
                   const int id,
                   const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;

    GodType type() const { return mType; }
    int id() const { return mId; }
private:
    const GodType mType;
    const int mId;
};

#endif // EPYRAMIDSTATUE_H
