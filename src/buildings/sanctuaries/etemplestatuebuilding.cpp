#include "etemplestatuebuilding.h"

eTempleStatueBuilding::eTempleStatueBuilding(
        const GodType god,
        const int id, GameBoard& board,
        const eCityId cid) :
    eSanctBuilding({{0, 0, 1}}, board,
                   eBuildingType::templeStatue,
                   1, 1, cid),
    mGod(god), mId(id) {

}

