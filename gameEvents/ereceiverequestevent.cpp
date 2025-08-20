#include "ereceiverequestevent.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "emessages.h"
#include "engine/ecityrequest.h"

eReceiveRequestEvent::eReceiveRequestEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::receiveRequest, branch, board),
    eCityEventValue(board) {
    const auto e1 = eLanguage::text("early");
    mEarlyTrigger = e::make_shared<eEventTrigger>(cid, e1, board);
    const auto e2 = eLanguage::text("comply");
    mComplyTrigger = e::make_shared<eEventTrigger>(cid, e2, board);
    const auto e3 = eLanguage::text("too_late");
    mTooLateTrigger = e::make_shared<eEventTrigger>(cid, e3, board);
    const auto e4 = eLanguage::text("refuse");
    mRefuseTrigger = e::make_shared<eEventTrigger>(cid, e4, board);

    addTrigger(mEarlyTrigger);
    addTrigger(mComplyTrigger);
    addTrigger(mTooLateTrigger);
    addTrigger(mRefuseTrigger);
}

eReceiveRequestEvent::~eReceiveRequestEvent() {
    const auto board = gameBoard();
    if(board) board->removeCityRequest(this);
}

const int gPostponeDays = 6*31;

void eReceiveRequestEvent::trigger() {
    chooseCity();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;

    if(isMainEvent() && mPostpone == 0) { // initial
        chooseType();
        chooseCount();
        board->addCityRequest(mainEvent<eReceiveRequestEvent>());
    }

    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fResourceType = mResource;
    ed.fResourceCount = mCount;
    ed.fTime = 6;
    ed.fGod = mGod;

    if(mFinish) {
        auto& msgs = eMessages::instance;
        if(mPostpone > 1) {
            ed.fType = eMessageEventType::resourceGranted;
            eEvent event;
            eReceiveRequestMessages* rrmsgs = nullptr;
            if(mRequestType == eReceiveRequestType::tribute) {
                event = eEvent::generalRequestTributeTooLate;
                rrmsgs = &msgs.fTributeRequest;
            } else if(mRequestType == eReceiveRequestType::general) {
                if(mCity->isRival()) {
                    event = eEvent::generalRequestRivalTooLate;
                    rrmsgs = &msgs.fGeneralRequestRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::generalRequestSubjectTooLate;
                    rrmsgs = &msgs.fGeneralRequestSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::generalRequestParentTooLate;
                    rrmsgs = &msgs.fGeneralRequestParentR;
                } else { // ally
                    event = eEvent::generalRequestAllyTooLate;
                    rrmsgs = &msgs.fGeneralRequestAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::famine) {
                if(mCity->isRival()) {
                    event = eEvent::famineRivalTooLate;
                    rrmsgs = &msgs.fFamineRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::famineSubjectTooLate;
                    rrmsgs = &msgs.fFamineSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::famineParentTooLate;
                    rrmsgs = &msgs.fFamineParentR;
                } else { // ally
                    event = eEvent::famineAllyTooLate;
                    rrmsgs = &msgs.fFamineAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::project) {
                if(mCity->isRival()) {
                    event = eEvent::projectRivalTooLate;
                    rrmsgs = &msgs.fProjectRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::projectSubjectTooLate;
                    rrmsgs = &msgs.fProjectSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::projectParentTooLate;
                    rrmsgs = &msgs.fProjectParentR;
                } else { // ally
                    event = eEvent::projectAllyTooLate;
                    rrmsgs = &msgs.fProjectAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::festival) {
                if(mCity->isRival()) {
                    event = eEvent::festivalRivalTooLate;
                    rrmsgs = &msgs.fFestivalRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::festivalSubjectTooLate;
                    rrmsgs = &msgs.fFestivalSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::festivalParentTooLate;
                    rrmsgs = &msgs.fFestivalParentR;
                } else { // ally
                    event = eEvent::festivalAllyTooLate;
                    rrmsgs = &msgs.fFestivalAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::financialWoes) {
                if(mCity->isRival()) {
                    event = eEvent::financialWoesRivalTooLate;
                    rrmsgs = &msgs.fFinancialWoesRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::financialWoesSubjectTooLate;
                    rrmsgs = &msgs.fFinancialWoesSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::financialWoesParentTooLate;
                    rrmsgs = &msgs.fFinancialWoesParentR;
                } else { // ally
                    event = eEvent::financialWoesAllyTooLate;
                    rrmsgs = &msgs.fFinancialWoesAllyS;
                }
            } else {
                assert(false);
            }
            board->event(event, ed);
            mCity->incAttitude(-5, pid);

            const auto& reason = rrmsgs->fTooLateReason;
            const auto me = mainEvent<eReceiveRequestEvent>();
            me->finished(*me->mTooLateTrigger, reason);
        } else {
            ed.fType = eMessageEventType::resourceGranted;
            eEvent event;
            eReceiveRequestMessages* rrmsgs = nullptr;
            if(mRequestType == eReceiveRequestType::tribute) {
                event = eEvent::generalRequestTributeComply;
                rrmsgs = &msgs.fTributeRequest;
            } else if(mRequestType == eReceiveRequestType::general) {
                if(mCity->isRival()) {
                    event = eEvent::generalRequestRivalComply;
                    rrmsgs = &msgs.fGeneralRequestRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::generalRequestSubjectComply;
                    rrmsgs = &msgs.fGeneralRequestSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::generalRequestParentComply;
                    rrmsgs = &msgs.fGeneralRequestParentR;
                } else { // ally
                    event = eEvent::generalRequestAllyComply;
                    rrmsgs = &msgs.fGeneralRequestAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::famine) {
                if(mCity->isRival()) {
                    event = eEvent::famineRivalComply;
                    rrmsgs = &msgs.fFamineRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::famineSubjectComply;
                    rrmsgs = &msgs.fFamineSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::famineParentComply;
                    rrmsgs = &msgs.fFamineParentR;
                } else { // ally
                    event = eEvent::famineAllyComply;
                    rrmsgs = &msgs.fFamineAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::project) {
                if(mCity->isRival()) {
                    event = eEvent::projectRivalComply;
                    rrmsgs = &msgs.fProjectRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::projectSubjectComply;
                    rrmsgs = &msgs.fProjectSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::projectParentComply;
                    rrmsgs = &msgs.fProjectParentR;
                } else { // ally
                    event = eEvent::projectAllyComply;
                    rrmsgs = &msgs.fProjectAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::festival) {
                if(mCity->isRival()) {
                    event = eEvent::festivalRivalComply;
                    rrmsgs = &msgs.fFestivalRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::festivalSubjectComply;
                    rrmsgs = &msgs.fFestivalSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::festivalParentComply;
                    rrmsgs = &msgs.fFestivalParentR;
                } else { // ally
                    event = eEvent::festivalAllyComply;
                    rrmsgs = &msgs.fFestivalAllyS;
                }
            } else if(mRequestType == eReceiveRequestType::financialWoes) {
                if(mCity->isRival()) {
                    event = eEvent::financialWoesRivalComply;
                    rrmsgs = &msgs.fFinancialWoesRivalD;
                } else if(mCity->isVassal() || mCity->isColony()) {
                    event = eEvent::financialWoesSubjectComply;
                    rrmsgs = &msgs.fFinancialWoesSubjectP;
                } else if(mCity->isParentCity()) {
                    event = eEvent::financialWoesParentComply;
                    rrmsgs = &msgs.fFinancialWoesParentR;
                } else { // ally
                    event = eEvent::financialWoesAllyComply;
                    rrmsgs = &msgs.fFinancialWoesAllyS;
                }
            } else {
                assert(false);
            }
            board->event(event, ed);
            mCity->incAttitude(10, pid);

            const auto& reason = rrmsgs->fComplyReason;
            const auto me = mainEvent<eReceiveRequestEvent>();
            me->finished(*me->mComplyTrigger, reason);
        }
        return;
    }

    if(mPostpone > 3) {
        ed.fType = eMessageEventType::resourceGranted;
        eEvent event;
        eReceiveRequestMessages* rrmsgs = nullptr;
        auto& msgs = eMessages::instance;
        if(mRequestType == eReceiveRequestType::tribute) {
            event = eEvent::generalRequestTributeRefuse;
            rrmsgs = &msgs.fTributeRequest;
        } else if(mRequestType == eReceiveRequestType::general) {
            if(mCity->isRival()) {
                event = eEvent::generalRequestRivalRefuse;
                rrmsgs = &msgs.fGeneralRequestRivalD;
            } else if(mCity->isVassal() || mCity->isColony()) {
                event = eEvent::generalRequestSubjectRefuse;
                rrmsgs = &msgs.fGeneralRequestSubjectP;
            } else if(mCity->isParentCity()) {
                event = eEvent::generalRequestParentRefuse;
                rrmsgs = &msgs.fGeneralRequestParentR;
            } else { // ally
                event = eEvent::generalRequestAllyRefuse;
                rrmsgs = &msgs.fGeneralRequestAllyS;
            }
        } else if(mRequestType == eReceiveRequestType::famine) {
            if(mCity->isRival()) {
                event = eEvent::famineRivalRefuse;
                rrmsgs = &msgs.fFamineRivalD;
            } else if(mCity->isVassal() || mCity->isColony()) {
                event = eEvent::famineSubjectRefuse;
                rrmsgs = &msgs.fFamineSubjectP;
            } else if(mCity->isParentCity()) {
                event = eEvent::famineParentRefuse;
                rrmsgs = &msgs.fFamineParentR;
            } else { // ally
                event = eEvent::famineAllyRefuse;
                rrmsgs = &msgs.fFamineAllyS;
            }
        } else if(mRequestType == eReceiveRequestType::project) {
            if(mCity->isRival()) {
                event = eEvent::projectRivalRefuse;
                rrmsgs = &msgs.fProjectRivalD;
            } else if(mCity->isVassal() || mCity->isColony()) {
                event = eEvent::projectSubjectRefuse;
                rrmsgs = &msgs.fProjectSubjectP;
            } else if(mCity->isParentCity()) {
                event = eEvent::projectParentRefuse;
                rrmsgs = &msgs.fProjectParentR;
            } else { // ally
                event = eEvent::projectAllyRefuse;
                rrmsgs = &msgs.fProjectAllyS;
            }
        } else if(mRequestType == eReceiveRequestType::festival) {
            if(mCity->isRival()) {
                event = eEvent::festivalRivalRefuse;
                rrmsgs = &msgs.fFestivalRivalD;
            } else if(mCity->isVassal() || mCity->isColony()) {
                event = eEvent::festivalSubjectRefuse;
                rrmsgs = &msgs.fFestivalSubjectP;
            } else if(mCity->isParentCity()) {
                event = eEvent::festivalParentRefuse;
                rrmsgs = &msgs.fFestivalParentR;
            } else { // ally
                event = eEvent::festivalAllyRefuse;
                rrmsgs = &msgs.fFestivalAllyS;
            }
        } else if(mRequestType == eReceiveRequestType::financialWoes) {
            if(mCity->isRival()) {
                event = eEvent::financialWoesRivalRefuse;
                rrmsgs = &msgs.fFinancialWoesRivalD;
            } else if(mCity->isVassal() || mCity->isColony()) {
                event = eEvent::financialWoesSubjectRefuse;
                rrmsgs = &msgs.fFinancialWoesSubjectP;
            } else if(mCity->isParentCity()) {
                event = eEvent::financialWoesParentRefuse;
                rrmsgs = &msgs.fFinancialWoesParentR;
            } else { // ally
                event = eEvent::financialWoesAllyRefuse;
                rrmsgs = &msgs.fFinancialWoesAllyS;
            }
        } else {
            assert(false);
        }
        board->event(event, ed);
        mCity->incAttitude(-15, pid);

        auto& reason = rrmsgs->fRefuseReason;
        const auto me = mainEvent<eReceiveRequestEvent>();
        me->finished(*me->mRefuseTrigger, reason);
        board->removeCityRequest(me);
        return;
    }

    const auto cids = board->playerCitiesOnBoard(pid);
    if(mResource == eResourceType::drachmas) {
        const int avCount = board->drachmas(pid);
        if(avCount >= mCount) {
            const auto cid = cids[0];
            ed.fA0 = [this, cid]() {
                dispatch(cid);
            };
        }
    } else {
        for(const auto cid : cids) {
            const int avCount = board->resourceCount(cid, mResource);
            ed.fCityNames[cid] = board->cityName(cid);
            ed.fCSpaceCount[cid] = avCount;

            if(avCount >= mCount) {
                ed.fCCA0[cid] = [this, cid]() { // dispatch now
                    dispatch(cid);
                };
            }
        }
    }

    if(mPostpone < 3) {
        ed.fA1 = [this, board]() { // postpone
            const auto e = e::make_shared<eReceiveRequestEvent>(
                               cityId(), eGameEventBranch::child, *board);
            e->set(*this, mPostpone + 1);
            const auto date = board->date() + gPostponeDays;
            e->initializeDate(date);
            addConsequence(e);
        };
    }

    ed.fA2 = [this, board]() { // refuse
        board->removeCityRequest(mainEvent<eReceiveRequestEvent>());
        const auto e = e::make_shared<eReceiveRequestEvent>(
                           cityId(), eGameEventBranch::child, *board);
        e->set(*this, 5);
        const auto date = board->date() + 31;
        e->initializeDate(date);
        addConsequence(e);
    };


    ed.fType = eMessageEventType::generalRequestGranted;
    if(mRequestType == eReceiveRequestType::tribute) {
        if(mPostpone == 0) { // initial
            board->event(eEvent::generalRequestTributeInitial, ed);
        } else if(mPostpone == 1) { // reminder
            board->event(eEvent::generalRequestTributeReminder, ed);
        } else if(mPostpone == 2) { // overdue
            board->event(eEvent::generalRequestTributeOverdue, ed);
        } else if(mPostpone == 3) { // warning
            board->event(eEvent::generalRequestTributeWarning, ed);
        }
    } else if(mRequestType == eReceiveRequestType::general) {
        if(mCity->isRival()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::generalRequestRivalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::generalRequestRivalReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::generalRequestRivalOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::generalRequestRivalWarning, ed);
            }
        } else if(mCity->isVassal() || mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::generalRequestSubjectInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::generalRequestSubjectReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::generalRequestSubjectOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::generalRequestSubjectWarning, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::generalRequestParentInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::generalRequestParentReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::generalRequestParentOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::generalRequestParentWarning, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::generalRequestAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::generalRequestAllyReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::generalRequestAllyOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::generalRequestAllyWarning, ed);
            }
        }
    } else if(mRequestType == eReceiveRequestType::famine) {
        if(mCity->isRival()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::famineRivalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::famineRivalReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::famineRivalOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::famineRivalWarning, ed);
            }
        } else if(mCity->isVassal() || mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::famineSubjectInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::famineSubjectReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::famineSubjectOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::famineSubjectWarning, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::famineParentInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::famineParentReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::famineParentOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::famineParentWarning, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::famineAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::famineAllyReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::famineAllyOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::famineAllyWarning, ed);
            }
        }
    } else if(mRequestType == eReceiveRequestType::general) {
        if(mCity->isRival()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::projectRivalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::projectRivalReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::projectRivalOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::projectRivalWarning, ed);
            }
        } else if(mCity->isVassal() || mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::projectSubjectInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::projectSubjectReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::projectSubjectOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::projectSubjectWarning, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::projectParentInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::projectParentReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::projectParentOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::projectParentWarning, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::projectAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::projectAllyReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::projectAllyOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::projectAllyWarning, ed);
            }
        }
    } else if(mRequestType == eReceiveRequestType::festival) {
        if(mCity->isRival()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::festivalRivalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::festivalRivalReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::festivalRivalOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::festivalRivalWarning, ed);
            }
        } else if(mCity->isVassal() || mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::festivalSubjectInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::festivalSubjectReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::festivalSubjectOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::festivalSubjectWarning, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::festivalParentInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::festivalParentReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::festivalParentOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::festivalParentWarning, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::festivalAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::festivalAllyReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::festivalAllyOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::festivalAllyWarning, ed);
            }
        }
    } else if(mRequestType == eReceiveRequestType::financialWoes) {
        if(mCity->isRival()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::financialWoesRivalInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::financialWoesRivalReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::financialWoesRivalOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::financialWoesRivalWarning, ed);
            }
        } else if(mCity->isVassal() || mCity->isColony()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::financialWoesSubjectInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::financialWoesSubjectReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::financialWoesSubjectOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::financialWoesSubjectWarning, ed);
            }
        } else if(mCity->isParentCity()) {
            if(mPostpone == 0) { // initial
                board->event(eEvent::financialWoesParentInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::financialWoesParentReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::financialWoesParentOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::financialWoesParentWarning, ed);
            }
        } else { // ally
            if(mPostpone == 0) { // initial
                board->event(eEvent::financialWoesAllyInitial, ed);
            } else if(mPostpone == 1) { // reminder
                board->event(eEvent::financialWoesAllyReminder, ed);
            } else if(mPostpone == 2) { // overdue
                board->event(eEvent::financialWoesAllyOverdue, ed);
            } else if(mPostpone == 3) { // warning
                board->event(eEvent::financialWoesAllyWarning, ed);
            }
        }
    }
}

std::string eReceiveRequestEvent::longName() const {
    auto tmpl = eLanguage::text("receive_request_long_name");
    eCountEventValue::longNameReplaceCount("%1", tmpl);
    eResourceEventValue::longNameReplaceResource("%2", tmpl);
    return tmpl;
}

void eReceiveRequestEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eResourceEventValue::write(dst);
    eCountEventValue::write(dst);
    eCityEventValue::write(dst);
    eGodEventValue::write(dst);
    dst << mRequestType;
    dst << mFinish;
    dst << mPostpone;
}

void eReceiveRequestEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eResourceEventValue::read(src);
    eCountEventValue::read(src);
    eCityEventValue::read(src, *gameBoard());
    eGodEventValue::read(src);
    src >> mRequestType;
    src >> mFinish;
    src >> mPostpone;
}

eCityRequest eReceiveRequestEvent::cityRequest() const {
    eCityRequest request;
    request.fCity = mCity;
    request.fType = mResource;
    request.fCount = mCount;
    return request;
}

void eReceiveRequestEvent::dispatch(const eCityId cid) {
    const auto board = gameBoard();
    if(!board) return;
    board->takeResource(cid, mResource, mCount);
    fulfillWithoutCost();
}

void eReceiveRequestEvent::fulfillWithoutCost() {
    const auto board = gameBoard();
    if(!board) return;
    board->removeCityRequest(mainEvent<eReceiveRequestEvent>());
    const auto e = e::make_shared<eReceiveRequestEvent>(*this);
    int postpone = mPostpone - 1;
    auto date = startDate();
    const auto currentDate = board->date();
    while(date <= currentDate) {
        date += gPostponeDays;
        postpone++;
    }
    e->set(*this, postpone, true);
    const auto edate = currentDate + 3*31;
    e->initializeDate(edate);
    clearConsequences();
    addConsequence(e);
}

void eReceiveRequestEvent::initialize(
        const int postpone,
        const eResourceType res,
        const int count,
        const stdsptr<eWorldCity> &c,
        const bool finish) {
    mPostpone = postpone;
    mResource = res;
    mCount = count;
    setSingleCity(c);
    mFinish = finish;
}

void eReceiveRequestEvent::set(eReceiveRequestEvent &src,
                               const int postpone,
                               const bool finish) {
    mResource = src.mResource;
    mCount = src.mCount;
    mCity = src.mCity;
    if(mCity) {
        const auto cid = mCity->cityId();
        const int i = static_cast<int>(cid);
        setMinCityId(i);
        setMaxCityId(i);
    }
    mGod = src.mGod;

    mPostpone = postpone;
    mFinish = finish;
}

void eReceiveRequestEvent::finished(eEventTrigger& t, const eReason& r) {
    const auto board = gameBoard();
    if(!board) return;
    const auto date = board->date();
    auto rFull = r.fFull;
    const auto amount = std::to_string(mCount);
    eStringHelpers::replaceAll(rFull, "[amount]", amount);
    const auto item = eResourceTypeHelpers::typeLongName(mResource);
    eStringHelpers::replaceAll(rFull, "[item]", item);
    t.trigger(*this, date, rFull);
}
