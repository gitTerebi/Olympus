#include "receive-tribute-event.h"
#include "engine/game-board.h"
#include "engine/eevent.h"
#include "engine/tribute.h"
#include "engine/eeventdata.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

ReceiveTributeEvent::ReceiveTributeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::receiveTribute, branch, board) {}

void ReceiveTributeEvent::initialize(const stdsptr<WorldCity>& c) {
    mCity = c;
}

void ReceiveTributeEvent::trigger() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = TributeHelpers::receiveTribute(*mCity);
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
            const int space = board->spaceForResource(cid, type);
            ed.fCSpaceCount[cid] = space;
            ed.fCityNames[cid] = board->cityName(cid);
            if(space >= count) {
                ed.fCityConditionalResponses[cid] = static_cast<int>(eResponse::accept);
            }
        }
    }
    if(!mPostponed) ed.fSecondaryResponse = static_cast<int>(eResponse::postpone);
    ed.fTertiaryResponse = static_cast<int>(eResponse::decline);
    mAwaitingResponse = true;
    board->event(eEvent::tributePaid, ed);
}

bool ReceiveTributeEvent::finished() const {
    return eGameEvent::finished() && !mAwaitingResponse;
}

void ReceiveTributeEvent::respond(const int response, const eCityId city) {
    mAwaitingResponse = false;
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

void ReceiveTributeEvent::accept(const eCityId city) {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = TributeHelpers::receiveTribute(*mCity);
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

void ReceiveTributeEvent::postpone() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    const auto tribute = TributeHelpers::receiveTribute(*mCity);
    eEventData ed(pid);
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = tribute.fType;
    ed.fResourceCount = tribute.fCount;
    board->event(eEvent::tributePostponed, ed);
    const auto e = e::make_shared<ReceiveTributeEvent>(
        board->currentCityId(), eGameEventBranch::root, *board);
    e->initialize(mCity);
    e->mPostponed = true;
    auto date = board->date();
    date.nextMonths(1);
    e->initializeDate(date);
    board->addRootGameEvent(e);
}

void ReceiveTributeEvent::decline() {
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto tribute = TributeHelpers::receiveTribute(*mCity);
    eEventData ed(playerId());
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = mCity;
    ed.fResourceType = tribute.fType;
    ed.fResourceCount = tribute.fCount;
    board->event(eEvent::tributeDeclined, ed);
}

std::string ReceiveTributeEvent::longName() const {
    auto tmpl = eLanguage::text("receive_tribute_from");
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", ctstr);
    return tmpl;
}

void ReceiveTributeEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.worldCityField("city", worldBoard(), mCity);
    ar.field("awaitingResponse", mAwaitingResponse, false);
    ar.field("postponed", mPostponed, false);
}
