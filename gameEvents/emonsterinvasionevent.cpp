#include "emonsterinvasionevent.h"

#include "elanguage.h"
#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"

eMonsterInvasionWarningEvent::eMonsterInvasionWarningEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::monsterInvasionWarning, branch, board) {}

void eMonsterInvasionWarningEvent::initialize(
        const eMonsterInvasionWarningType type) {
    mType = type;
}

void eMonsterInvasionWarningEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    eEventData ed(cid);

    const auto p = parent();
    if(const auto i = dynamic_cast<eMonsterInvasionEventBase*>(p)) {
        i->setWarned(true);
        ed.fMonster = i->monsterType();
    }

    switch(mType) {
    case eMonsterInvasionWarningType::warning36: {
        board->event(eEvent::monsterInvasion36, ed);
    } break;
    case eMonsterInvasionWarningType::warning24: {
        board->event(eEvent::monsterInvasion24, ed);
    } break;
    case eMonsterInvasionWarningType::warning12: {
        board->event(eEvent::monsterInvasion12, ed);
    } break;
    case eMonsterInvasionWarningType::warning6: {
        board->event(eEvent::monsterInvasion6, ed);
    } break;
    case eMonsterInvasionWarningType::warning1: {
        board->event(eEvent::monsterInvasion1, ed);
    } break;
    }
}

std::string eMonsterInvasionWarningEvent::longName() const {
    return eLanguage::zeusText(182, 2);
}

void eMonsterInvasionWarningEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst << mType;
}

void eMonsterInvasionWarningEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src >> mType;
}

eMonsterInvasionEvent::eMonsterInvasionEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eMonsterInvasionEventBase(cid, eGameEventType::monsterInvasion,
                              branch, board) {}

void eMonsterInvasionEvent::pointerCreated() {
    const auto warnTypes = {
        eMonsterInvasionWarningType::warning36,
        eMonsterInvasionWarningType::warning24,
        eMonsterInvasionWarningType::warning12,
        eMonsterInvasionWarningType::warning6,
        eMonsterInvasionWarningType::warning1
    };
    const auto cid = cityId();
    for(const auto w : warnTypes) {
        int months;
        switch(w) {
        case eMonsterInvasionWarningType::warning36:
            months = 36;
            break;
        case eMonsterInvasionWarningType::warning24:
            months = 24;
            break;
        case eMonsterInvasionWarningType::warning12:
            months = 12;
            break;
        case eMonsterInvasionWarningType::warning6:
            months = 6;
            break;
        case eMonsterInvasionWarningType::warning1:
            months = 1;
            break;
        }
        const int daysBefore = 31*months;
        const auto e = e::make_shared<eMonsterInvasionWarningEvent>(
                           cid, eGameEventBranch::child, *gameBoard());
        e->initialize(w);
        addWarning(daysBefore, e);
    }
}

void eMonsterInvasionEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;

    const auto monster = triggerBase();

    const auto cid = cityId();
    eEventData ed(cid);
    ed.fChar = monster;
    ed.fTile = monster->tile();
    ed.fMonster = mMonster;
    board->event(eEvent::monsterInvasion, ed);

    const auto& inst = eMessages::instance;
    const auto gm = inst.monsterMessages(mMonster);
    const auto& m = gm->fToSlainReason;
    const auto heroType = eMonster::sSlayer(mMonster);
    board->allowHero(cid, heroType, m);
}

std::string eMonsterInvasionEvent::longName() const {
    return eLanguage::zeusText(182, 2);
}
