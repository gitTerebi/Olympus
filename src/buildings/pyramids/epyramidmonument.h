#ifndef EPYRAMIDMONUMENT_H
#define EPYRAMIDMONUMENT_H

#include "epyramidelement.h"

enum class GodType;

class ePyramidMonument : public ePyramidElement {
public:
    ePyramidMonument(const std::vector<eSanctCost>& cost,
                     GameBoard& board,
                     const int elevation,
                     const GodType type,
                     const int id,
                     const eCityId cid);
    ePyramidMonument(ePyramid* const pyramid,
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

#endif // EPYRAMIDMONUMENT_H
