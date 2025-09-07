#include "ereinforcementsevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "enumbers.h"

eReinforcementsEvent::eReinforcementsEvent(const eCityId cid,
                                           const eGameEventBranch branch,
                                           eGameBoard& board) :
    eArmyEventBase(cid, eGameEventType::reinforcementsEvent, branch, board) {}

void eReinforcementsEvent::initialize(const eEnlistedForces& forces,
                                      const stdsptr<eWorldCity>& city) {
    mForces = forces;
    mCity = city;
}

void eReinforcementsEvent::trigger() {
    removeArmyEvent();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    const auto fromCid = mCity->cityId();
    const auto pid = board->cityIdToPlayerId(cid);
    const auto fromPid = board->cityIdToPlayerId(fromCid);
    const auto c = board->boardCityWithId(cid);
    if(!c || pid != fromPid) {
        planArmyReturn(fromCid, eNumbers::sReinforcementsTravelTime);
        return;
    }
    const auto entryPoint = board->entryPoint(cid);
    if(!entryPoint) return;

    eEventData ed(cid);
    board->event(eEvent::aidArrives, ed);
    ed.fCity = mCity;
    ed.fTile = entryPoint;

    for(const auto& b : mForces.fSoldiers) {
        b->setOnCityId(cid);
        b->setMilitaryAid(true);
        b->backFromHome();
        for(int i = 0; i < eNumbers::sSoldiersPerBanner && i < b->count(); i++) {
            b->createSoldier(entryPoint);
        }
    }

    c->addReinforcements(fromCid, mForces);
}
