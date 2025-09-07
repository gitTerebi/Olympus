#include "earmyeventbase.h"

#include "engine/egameboard.h"

#include "earmyreturnevent.h"
#include "enumbers.h"

eArmyEventBase::eArmyEventBase(const eCityId cid,
                               const eGameEventType type,
                               const eGameEventBranch branch,
                               eGameBoard& board) :
    eGameEvent(cid, type, branch, board) {
    board.addArmyEvent(this);
}

eArmyEventBase::~eArmyEventBase() {
    removeArmyEvent();
}

void eArmyEventBase::removeArmyEvent() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeArmyEvent(this);
}

void eArmyEventBase::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    mForces.write(dst);
    dst.writeCity(mCity.get());
}

void eArmyEventBase::read(eReadStream& src) {
    eGameEvent::read(src);
    const auto board = gameBoard();
    mForces.read(*board, src);
    src.readCity(board, [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
}

void eArmyEventBase::planArmyReturn() {
    const auto cid = cityId();
    planArmyReturn(cid, eNumbers::sArmyTravelTime);
}

void eArmyEventBase::planArmyReturn(const eCityId cid, const int travelTime) {
    const auto board = gameBoard();
    if(!board) return;
    const auto e = e::make_shared<eArmyReturnEvent>(
        cid, eGameEventBranch::child, *board);
    const auto boardDate = board->date();
    const auto date = boardDate + travelTime;
    e->initializeDate(date, travelTime, 1);
    e->initialize(mForces, mCity);
    addConsequence(e);
}
