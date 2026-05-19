#include "ereceivetributeevent.h"
#include "engine/e-game-board.h"
#include "engine/eevent.h"
#include "engine/e-tribute.h"
#include "engine/eeventdata.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

eReceiveTributeEvent::eReceiveTributeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::receiveTribute, branch, board) {}

void eReceiveTributeEvent::initialize(const stdsptr<eWorldCity>& c) {
    mCity = c;
}

void eReceiveTributeEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = eTributeHelpers::receiveTribute(*mCity);
    const auto type = tribute.fType;
    const int count = tribute.fCount;
    eEventData ed(pid);
    ed.fType = eMessageEventType::requestTributeGranted;
    ed.fCity = mCity;
    ed.fEventRuntimeId = runtimeId();
    ed.fResourceType = type;
    ed.fResourceCount = count;
    if(type == eResourceType::drachmas) {
        ed.fPrimaryResponse = static_cast<int>(eResponse::accept);
    } else {
        const auto cids = board->playerCitiesOnBoard(pid);
        for(const auto cid : cids) {
            ed.fCSpaceCount[cid] = board->spaceForResource(cid, type);
            ed.fCityNames[cid] = board->cityName(cid);
            ed.fCityConditionalResponses[cid] = static_cast<int>(eResponse::accept);
        }
    }
    ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    ed.fTertiaryResponse = static_cast<int>(eResponse::decline);
    board->event(eEvent::tributePaid, ed);
}

void eReceiveTributeEvent::respond(const int response, const eCityId city) {
    switch(static_cast<eResponse>(response)) {
    case eResponse::accept:
        accept(city);
        break;
    case eResponse::postpone:
        postpone();
        break;
    case eResponse::decline:
        decline();
        break;
    }
}

void eReceiveTributeEvent::accept(const eCityId city) {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = eTributeHelpers::receiveTribute(*mCity);
    const auto type = tribute.fType;
    const int count = tribute.fCount;
    if(type == eResourceType::drachmas) {
        const auto p = board->boardPlayerWithId(pid);
        if(p) p->incDrachmas(count, eFinanceTarget::tributeReceived);
        return;
    }
    const int a = board->addResource(city, type, count);
    if(a == count) return;
    eEventData ed(pid);
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = type;
    ed.fResourceCount = a;
    board->event(eEvent::tributeAccepted, ed);
}

void eReceiveTributeEvent::postpone() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = eTributeHelpers::receiveTribute(*mCity);
    eEventData ed(pid);
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = tribute.fType;
    ed.fResourceCount = tribute.fCount;
    board->event(eEvent::tributePostponed, ed);
    const auto e = e::make_shared<eReceiveTributeEvent>(
        board->currentCityId(), eGameEventBranch::root, *board);
    e->initialize(mCity);
    auto date = board->date();
    date.nextYears(1);
    e->initializeDate(date);
    board->addRootGameEvent(e);
}

void eReceiveTributeEvent::decline() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto tribute = eTributeHelpers::receiveTribute(*mCity);
    eEventData ed(playerId());
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = tribute.fType;
    ed.fResourceCount = tribute.fCount;
    board->event(eEvent::tributeDeclined, ed);
}

std::string eReceiveTributeEvent::longName() const {
    auto tmpl = eLanguage::text("receive_tribute_from");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

void eReceiveTributeEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.worldCityField("city", worldBoard(), mCity);
}
