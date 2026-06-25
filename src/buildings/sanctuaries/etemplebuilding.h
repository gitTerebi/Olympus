#ifndef ETEMPLEBUILDING_H
#define ETEMPLEBUILDING_H

#include "esanctbuilding.h"

class SaveArchive;

class eTempleBuilding : public eSanctBuilding {
public:
    eTempleBuilding(GameBoard& board, const eCityId cid);
    eTempleBuilding(const int id, GameBoard& board,
                    const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize) const override { return nullptr; }
    int id() const { return mId; }

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    int mId = 0;
};

#endif // ETEMPLEBUILDING_H
