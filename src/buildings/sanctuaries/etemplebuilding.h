#ifndef ETEMPLEBUILDING_H
#define ETEMPLEBUILDING_H

#include "esanctbuilding.h"

class eSaveArchive;

class eTempleBuilding : public eSanctBuilding {
public:
    eTempleBuilding(GameBoard& board, const eCityId cid);
    eTempleBuilding(const int id, GameBoard& board,
                    const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize) const override { return nullptr; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mId;
};

#endif // ETEMPLEBUILDING_H
