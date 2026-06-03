#include "etemplebuilding.h"
#include "fileIO/esavearchive.h"

#include "esanctbuilding.h"

eTempleBuilding::eTempleBuilding(GameBoard& board,
                                 const eCityId cid) :
    eSanctBuilding({{2, 2, 0}, {1, 1, 0}, {2, 2, 0}}, board,
                   eBuildingType::temple, 4, 4, cid) {}

eTempleBuilding::eTempleBuilding(
        const int id, GameBoard& board,
        const eCityId cid) :
    eTempleBuilding(board, cid) {
    mId = id;
    setEnabled(true);
}

void eTempleBuilding::serializeFields(eSaveArchive& ar) {
    eSanctBuilding::serializeFields(ar);
    ar.field("id", mId);
}

