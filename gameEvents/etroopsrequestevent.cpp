#include "etroopsrequestevent.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"

#include "etroopsrequestfulfilledevent.h"

eTroopsRequestEvent::eTroopsRequestEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::troopsRequest, branch, board),
    eCityEventValue(board) {
    const auto e1 = eLanguage::text("early");
    mEarlyTrigger = e::make_shared<eEventTrigger>(cid, e1, board);
    const auto e2 = eLanguage::text("comply");
    mComplyTrigger = e::make_shared<eEventTrigger>(cid, e2, board);
    const auto e3 = eLanguage::text("too_late");
    mTooLateTrigger = e::make_shared<eEventTrigger>(cid, e3, board);
    const auto e4 = eLanguage::text("refuse");
    mRefuseTrigger = e::make_shared<eEventTrigger>(cid, e4, board);
    const auto e5 = eLanguage::text("lost_battle");
    mLostBattleTrigger = e::make_shared<eEventTrigger>(cid, e5, board);

    addTrigger(mEarlyTrigger);
    addTrigger(mComplyTrigger);
    addTrigger(mTooLateTrigger);
    addTrigger(mRefuseTrigger);
    addTrigger(mLostBattleTrigger);
}

eTroopsRequestEvent::~eTroopsRequestEvent() {
    const auto board = gameBoard();
    if(board) board->removeCityTroopsRequest(this);
}

void eTroopsRequestEvent::set(
        eTroopsRequestEvent& src,
        const int postpone,
        const bool finish) {
    mType = src.mType;
    mEffect = src.mEffect;
    setSingleCity(src.mCity);
    mAttackingCity = src.mAttackingCity;

    mPostpone = postpone;
    mFinish = finish;
}

std::string eTroopsRequestEvent::longName() const {
    return eLanguage::zeusText(290, 6);
}

void eTroopsRequestEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eCityEventValue::write(dst);
    eMonsterEventValue::write(dst);
    eAttackingCityEventValue::write(dst);
    dst << mType;
    dst << mEffect;
    dst << mFinish;
    dst << mPostpone;
}

void eTroopsRequestEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eCityEventValue::read(src, *gameBoard());
    eMonsterEventValue::read(src);
    eAttackingCityEventValue::read(src, *gameBoard());
    src >> mType;
    src >> mEffect;
    src >> mFinish;
    src >> mPostpone;
}

const int gPostponeDays = 6*31;

void eTroopsRequestEvent::trigger() {
    chooseCity();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fRivalCity = mAttackingCity;
    ed.fTime = 6;

    if(mFinish) {
        if(mPostpone > 2) {
            lost();
        } else {
            won();
        }
        return;
    }

    ed.fCA0 = [this](const eAction& close) { // dispatch now
        dispatch(close);
    };

    if(mPostpone < 2) {
        ed.fA1 = [this, board]() { // postpone
            const auto e = e::make_shared<eTroopsRequestEvent>(
                               cityId(), eGameEventBranch::child, *board);
            e->set(*this, mPostpone + 1);
            const auto date = board->date() + gPostponeDays;
            e->initializeDate(date);
            addConsequence(e);
        };
    }

    ed.fA2 = [this, board]() { // refuse
        board->removeCityTroopsRequest(mainEvent<eTroopsRequestEvent>());
        const auto e = e::make_shared<eTroopsRequestEvent>(
                           cityId(), eGameEventBranch::child, *board);
        e->set(*this, 5, true);
        const auto date = board->date() + 31;
        e->initializeDate(date);
        addConsequence(e);
    };


    ed.fType = eMessageEventType::troopsRequest;
    if(mPostpone == 0) { // initial
        board->addCityTroopsRequest(mainEvent<eTroopsRequestEvent>());
    }
    switch(mType) {
    case eTroopsRequestEventType::cityUnderAttack: {
        if(mCity->isVassal()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestVassalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestVassalFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestVassalLastReminder, ed);
            }
        } else if(mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestColonyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestColonyFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestColonyLastReminder, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestParentCityInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestParentCityFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestParentCityLastReminder, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestAllyFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestAllyLastReminder, ed);
            }
        }
    } break;
    case eTroopsRequestEventType::cityAttacksRival: {
        if(mCity->isVassal()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestAttackVassalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestAttackVassalFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestAttackVassalLastReminder, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsRequestAttackAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsRequestAttackAllyFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsRequestAttackAllyLastReminder, ed);
            }
        }
    } break;
    case eTroopsRequestEventType::greekCityTerrorized: {
        ed.fMonster = mMonster;
        if(mCity->isVassal()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsMonsterRequestVassalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsMonsterRequestVassalFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsMonsterRequestVassalLastReminder, ed);
            }
        } else if(mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsMonsterRequestColonyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsMonsterRequestColonyFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsMonsterRequestColonyLastReminder, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsMonsterRequestParentCityInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsMonsterRequestParentCityFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsMonsterRequestParentCityLastReminder, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::troopsMonsterRequestAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::troopsMonsterRequestAllyFirstReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::troopsMonsterRequestAllyLastReminder, ed);
            }
        }
    } break;
    }
}

void eTroopsRequestEvent::dispatch(const eAction& close) {
    const auto board = gameBoard();
    if(!board) return;
    board->requestForces([this, board, close](
                         const eEnlistedForces& f,
                         const eResourceType) {
        board->enlistForces(f);
        board->removeCityTroopsRequest(mainEvent<eTroopsRequestEvent>());
        const auto e = e::make_shared<eTroopsRequestFulfilledEvent>(
                           cityId(), eGameEventBranch::child, *board);
        const auto currentDate = board->date();
        e->initialize(f, mCity, mAttackingCity);
        const auto edate = currentDate + 3*31;
        e->initializeDate(edate);
        clearConsequences();
        addConsequence(e);
        if(close) close();
    }, {}, {mCity, mAttackingCity});
}

void eTroopsRequestEvent::won() {
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fRivalCity = mAttackingCity;
    ed.fType = eMessageEventType::common;

    auto& msgs = eMessages::instance;
    eTroopsRequestedMessages* rrmsgs = nullptr;

    switch(mType) {
    case eTroopsRequestEventType::cityUnderAttack: {
        if(mCity->isVassal()) {
            rrmsgs = &msgs.fVassalTroopsRequest;
        } else if(mCity->isColony()) {
            rrmsgs = &msgs.fColonyTroopsRequest;
        } else if(mCity->isParentCity()) {
            rrmsgs = &msgs.fParentCityTroopsRequest;
        } else { // ally
            rrmsgs = &msgs.fAllyTroopsRequest;
        }
        board->event(eEvent::troopsRequestAttackAverted, ed);
    } break;
    case eTroopsRequestEventType::cityAttacksRival: {
        eEvent event;
        if(mCity->isVassal()) {
            event = eEvent::vassalConqueresRival;
            rrmsgs = &msgs.fVassalTroopsAttackRequest;
        } else { // ally
            event = eEvent::allyConqueresRival;
            rrmsgs = &msgs.fAllyTroopsAttackRequest;
        }
        board->event(event, ed);

        switch(mEffect) {
        case eTroopsRequestEventEffect::unaffected: {
        } break;
        case eTroopsRequestEventEffect::destroyed: {
            mAttackingCity->setVisible(false);
        } break;
        case eTroopsRequestEventEffect::conquered: {
            mAttackingCity->setRelationship(eForeignCityRelationship::ally);
            const auto pid = mAttackingCity->playerId();
            board->setPlayerTeam(pid, eTeamId::team0);
        } break;
        }
    } break;
    case eTroopsRequestEventType::greekCityTerrorized: {
        ed.fMonster = mMonster;
        if(mCity->isVassal()) {
            rrmsgs = &msgs.fVassalTroopsMonsterRequest;
        } else if(mCity->isColony()) {
            rrmsgs = &msgs.fColonyTroopsMonsterRequest;
        } else if(mCity->isParentCity()) {
            rrmsgs = &msgs.fParentCityTroopsMonsterRequest;
        } else { // ally
            rrmsgs = &msgs.fAllyTroopsMonsterRequest;
        }
        board->event(eEvent::troopsMonsterRequestAttackAverted, ed);
    } break;
    }
    mCity->incAttitude(10, pid);

    const auto& reason = rrmsgs->fComplyReason;
    const auto me = mainEvent<eTroopsRequestEvent>();
    me->finished(*me->mComplyTrigger, reason);
}

void eTroopsRequestEvent::lost() {
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fRivalCity = mAttackingCity;
    ed.fType = eMessageEventType::common;

    auto& msgs = eMessages::instance;
    eEvent event;
    eTroopsRequestedMessages* rrmsgs = nullptr;
    switch(mType) {
    case eTroopsRequestEventType::cityUnderAttack: {
        if(mCity->isVassal()) {
            event = eEvent::troopsRequestVassalConquered;
            rrmsgs = &msgs.fVassalTroopsRequest;
        } else if(mCity->isColony()) {
            event = eEvent::troopsRequestColonyConquered;
            rrmsgs = &msgs.fColonyTroopsRequest;
        } else if(mCity->isParentCity()) {
            event = eEvent::troopsRequestParentCityConquered;
            rrmsgs = &msgs.fParentCityTroopsRequest;
            board->setEpisodeLost();
        } else { // ally
            event = eEvent::troopsRequestAllyConquered;
            rrmsgs = &msgs.fAllyTroopsRequest;
        }

        switch(mEffect) {
        case eTroopsRequestEventEffect::unaffected: {
        } break;
        case eTroopsRequestEventEffect::destroyed: {
            mCity->setVisible(false);
        } break;
        case eTroopsRequestEventEffect::conquered: {
            if(mCity->isColony()) {
                mCity->setConqueredBy(mAttackingCity);
            } else {
                mCity->setRelationship(eForeignCityRelationship::rival);
                const auto pid = mAttackingCity->playerId();
                board->setPlayerTeam(pid, eTeamId::team1);
            }
        } break;
        }
    } break;
    case eTroopsRequestEventType::cityAttacksRival: {
        return;
    } break;
    case eTroopsRequestEventType::greekCityTerrorized: {
        ed.fMonster = mMonster;
        if(mCity->isVassal()) {
            event = eEvent::troopsMonsterRequestVassalConquered;
            rrmsgs = &msgs.fVassalTroopsMonsterRequest;
        } else if(mCity->isColony()) {
            event = eEvent::troopsMonsterRequestColonyConquered;
            rrmsgs = &msgs.fColonyTroopsMonsterRequest;
        } else if(mCity->isParentCity()) {
            event = eEvent::troopsMonsterRequestParentCityConquered;
            rrmsgs = &msgs.fParentCityTroopsMonsterRequest;
            board->setEpisodeLost();
        } else { // ally
            event = eEvent::troopsMonsterRequestAllyConquered;
            rrmsgs = &msgs.fAllyTroopsMonsterRequest;
        }

        switch(mEffect) {
        case eTroopsRequestEventEffect::unaffected: {
        } break;
        case eTroopsRequestEventEffect::destroyed: {
            mCity->setVisible(false);
        } break;
        case eTroopsRequestEventEffect::conquered: {
        } break;
        }
    } break;
    }
    board->event(event, ed);
    mCity->incAttitude(-25, pid);

    const auto& reason = rrmsgs->fLostBattleReason;
    const auto me = mainEvent<eTroopsRequestEvent>();
    me->finished(*me->mLostBattleTrigger, reason);
}

void eTroopsRequestEvent::finished(eEventTrigger& t, const eReason& r) {
    const auto board = gameBoard();
    if(!board) return;
    const auto date = board->date();
    t.trigger(*this, date, r.fFull);
}
