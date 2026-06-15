#ifndef ETEMPLESTATUEBUILDING_H
#define ETEMPLESTATUEBUILDING_H

#include "characters/gods/god.h"

#include "esanctbuilding.h"

class eTempleStatueBuilding : public eSanctBuilding {
public:
    eTempleStatueBuilding(const GodType god,
                          const int id, GameBoard& board,
                          const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize) const override { return nullptr; }

    GodType godType() const { return mGod; }
    int id() const { return mId; }
private:
    const GodType mGod;
    const int mId;
};

#endif // ETEMPLESTATUEBUILDING_H
