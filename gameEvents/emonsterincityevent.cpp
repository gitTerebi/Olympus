#include "emonsterincityevent.h"

#include "elanguage.h"
#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"

eMonsterInCityEvent::eMonsterInCityEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eMonsterInvasionEventBase(cid,
                              eGameEventType::monsterInCity,
                              branch, board) {}

void eMonsterInCityEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;

    const auto monster = triggerBase();

    const auto cid = cityId();
    eEventData ed(cid);
    ed.fChar = monster;
    ed.fTile = monster->tile();
    ed.fMonster = mMonster;
    board->event(eEvent::monsterInCity, ed);

    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(mMonster);
    const auto& m = gm->fInCityReason;
    const auto heroType = eMonster::sSlayer(mMonster);
    board->allowHero(cid, heroType, m);
}

std::string eMonsterInCityEvent::longName() const {
    return eLanguage::zeusText(182, 0);
}
