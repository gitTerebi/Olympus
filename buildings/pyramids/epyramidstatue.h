#ifndef EPYRAMIDSTATUE_H
#define EPYRAMIDSTATUE_H

#include "epyramidelement.h"

enum class eGodType;

class ePyramidStatue : public ePyramidElement {
public:
    ePyramidStatue(const std::vector<eSanctCost>& cost,
                   eGameBoard& board,
                   const int elevation,
                   const eGodType type,
                   const int id,
                   const eCityId cid);
    ePyramidStatue(ePyramid* const pyramid,
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

#endif // EPYRAMIDSTATUE_H
