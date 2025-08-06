#ifndef EPYRAMIDMONUMENT_H
#define EPYRAMIDMONUMENT_H

#include "epyramidelement.h"

enum class eGodType;

class ePyramidMonument : public ePyramidElement {
public:
    ePyramidMonument(const std::vector<eSanctCost>& cost,
                     eGameBoard& board,
                     const int elevation,
                     const eGodType type,
                     const int id,
                     const eCityId cid);
    ePyramidMonument(ePyramid* const pyramid,
                     eGameBoard& board,
                     const int elevation,
                     const eGodType type,
                     const int id,
                     const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize size) const override;

    eGodType type() const { return mType; }
    int id() const { return mId; }
private:
    const eGodType mType;
    const int mId;
};

#endif // EPYRAMIDMONUMENT_H
