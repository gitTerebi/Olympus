#include "emonsterunleashedevent.h"

#include "elanguage.h"
#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"

eMonsterUnleashedEvent::eMonsterUnleashedEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eMonsterInvasionEventBase(cid,
                              eGameEventType::monsterUnleashed, branch,
                              board) {}

void eMonsterUnleashedEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;

    bool validGod = false;
    const auto godType = eMonster::sMonsterSender(mMonster, &validGod);
    if(!validGod) return;

    const auto monster = triggerBase();

    eEventData ed(cityId());
    ed.fChar = monster;
    ed.fTile = monster->tile();
    ed.fGod = godType;
    ed.fMonster = mMonster;
    board->event(eEvent::godMonsterUnleash, ed);

    const auto& inst = eMessages::instance;
    const auto gm = inst.godMessages(godType);
    const auto& m = gm->fMonster;
    const auto heroType = eMonster::sSlayer(mMonster);
    const auto cid = cityId();
    board->allowHero(cid, heroType, m.fReason);
}

std::string eMonsterUnleashedEvent::longName() const {
    return eLanguage::zeusText(182, 1);
}
