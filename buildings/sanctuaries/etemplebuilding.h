#ifndef ETEMPLEBUILDING_H
#define ETEMPLEBUILDING_H

#include "esanctbuilding.h"

class eSaveArchive;

class eTempleBuilding : public eSanctBuilding {
public:
    eTempleBuilding(GameBoard& board, const eCityId cid);
    eTempleBuilding(const int id, GameBoard& board,
                    const eCityId cid);

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

    std::vector<eOverlay>
    getOverlays(const eTileSize size) const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool hasNeighbour() const;

    int rotatedId() const;

    mutable bool mUpdateHasNeighbour = true;
    mutable bool mHasNeighbour = false;
    int mId;
};

#endif // ETEMPLEBUILDING_H
