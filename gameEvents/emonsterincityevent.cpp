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

    eEventData ed(cityId());
    ed.fChar = monster;
    ed.fTile = monster->tile();
    ed.fMonster = type();
    board->event(eEvent::monsterInCity, ed);

    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(type());
    const auto& m = gm->fInCityReason;
    const auto heroType = eMonster::sSlayer(type());
    const auto cid = cityId();
    board->allowHero(cid, heroType, m);
}

std::string eMonsterInCityEvent::longName() const {
    return eLanguage::text("monster_in_city_long_name");
}
