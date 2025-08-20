#include "esupplychangeevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eSupplyChangeEvent::eSupplyChangeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eSupplyDemandChangeEvent(cid, eGameEventType::supplyChange,
                             branch, board) {}

void eSupplyChangeEvent::trigger() {
    chooseCity();
    const auto city = this->city();
    if(!city) return;
    chooseType();
    chooseCount();
    const auto type = this->resourceType();
    const int by = this->count();
    auto& sells = city->sells();
    for(auto& s : sells) {
        if(s.fType != type) continue;
        s.fMax += by;
        break;
    }
    const auto board = gameBoard();
    if(!board) return;
    eEventData ed((ePlayerCityTarget()));
    ed.fCity = city;
    ed.fResourceType = type;
    const auto e = by > 0 ? eEvent::supplyIncrease :
                            eEvent::supplyDecrease;
    board->event(e, ed);
}

std::string eSupplyChangeEvent::longName() const {
    auto tmpl = eLanguage::text("supply_change_long_name");
    longNameReplaceResource("%1", tmpl);
    longNameReplaceCity("%2", tmpl);
    return tmpl;
}
