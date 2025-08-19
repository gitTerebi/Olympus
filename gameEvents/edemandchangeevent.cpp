#include "edemandchangeevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eDemandChangeEvent::eDemandChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eSupplyDemandChangeEvent(cid, eGameEventType::demandChange,
                             branch, board) {}

void eDemandChangeEvent::trigger() {
    const auto city = this->city();
    if(!city) return;
    chooseType();
    chooseCount();
    const auto type = this->resourceType();
    const int by = this->count();
    auto& buys = city->buys();
    for(auto& b : buys) {
        if(b.fType != type) continue;
        b.fMax += by;
        break;
    }
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    ed.fResourceType = type;
    const auto e = by > 0 ? eEvent::demandIncrease :
                            eEvent::demandDecrease;
    board->event(e, ed);
}

std::string eDemandChangeEvent::longName() const {
    auto tmpl = eLanguage::text("demand_change_long_name");
    longNameReplaceResource("%1", tmpl);
    longNameReplaceCity("%2", tmpl);
    return tmpl;
}
