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
    planArmyReturn(eNumbers::sArmyTravelTime);
}

void eArmyEventBase::planArmyReturn(const int travelTime) {
    const auto board = gameBoard();
    if(!board) return;
    std::map<eCityId, eEnlistedForces> forces;
    for(const auto& s : mForces.fSoldiers) {
        const auto cid = s->cityId();
        forces[cid].fSoldiers.push_back(s);
    }
    for(const auto& s : mForces.fHeroes) {
        forces[s.first].fHeroes.push_back(s);
    }
    if(mForces.fAres) {
        const auto cid = mForces.fAresCity;
        forces[cid].fAres = true;
        forces[cid].fAresCity = cid;
    }
    if(forces.empty() && !mForces.fAllies.empty()) {
        const auto cid = cityId();
        forces[cid].fAllies = mForces.fAllies;
    } else {
        const auto cid = forces.begin()->first;
        forces[cid].fAllies = mForces.fAllies;
    }
    for(const auto& f : forces) {
        const auto e = e::make_shared<eArmyReturnEvent>(
            f.first, eGameEventBranch::root, *board);
        const auto boardDate = board->date();
        const auto date = boardDate + travelTime;
        e->initializeDate(date, travelTime, 1);
        e->initialize(f.second, mCity);
        addConsequence(e);
    }
}
