#include "soldier-action.h"
#include "fileIO/esavearchive.h"

#include "characters/esoldier.h"
#include "engine/e-game-board.h"

#include <math.h>

#include "characters/esoldierbanner.h"

#include "emovetoaction.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "buildings/sanctuaries/etemplebuilding.h"
#include "ekillcharacterfinishfail.h"

eSoldierAction::eSoldierAction(eCharacter* const c) :
    eFightingAction(c, eCharActionType::soldierAction) {}

bool eSoldierAction::decide() {
    return true;
}

void eSoldierAction::increment(const int by) {
    if(mSpreadPeriod && currentAction()) {
        return eComplexAction::increment(by);
    } else {
        mSpreadPeriod = false;
    }

    if(isAttacking() && tooFarFromBanner()) {
        cancelAttack();
        const auto s = static_cast<eSoldier*>(character());
        const auto b = s->banner();
        if(b) {
            goBackToBanner(b->soldierOrientation());
            return eComplexAction::increment(by);
        }
    }

    const auto r = lookForEnemy(by);

    if(r != eLookForEnemyState::none) return;

    if(!currentAction()) {
        mGoToBannerCountdown -= by;
        if(mGoToBannerCountdown < 0) {
            mGoToBannerCountdown = 250;
            const stdptr<eSoldierAction> tptr(this);
            const auto taskFinished = [tptr]() {
                if(!tptr) return;
                tptr->mGoToBannerCountdown = 100;
            };
            const auto taskFindFailed = [tptr]() {
                if(!tptr) return;
                tptr->mGoToBannerCountdown = 1000;
            };

            const auto c = character();
            const auto s = static_cast<eSoldier*>(c);
            const auto b = s->banner();
            if(b) {
                goBackToBanner(b->soldierOrientation(),
                               taskFindFailed, taskFinished);
            }
        }
    }

    eComplexAction::increment(by);
}

bool eSoldierAction::tooFarFromBanner() const {
    const auto c = character();
    const auto s = static_cast<eSoldier*>(c);
    const auto b = s->banner();
    if(!b || !b->tile() || !c->tile()) return false;
    const int dx = c->tile()->x() - b->tile()->x();
    const int dy = c->tile()->y() - b->tile()->y();
    const int leash = 8 + c->range()/2;
    return dx*dx + dy*dy > leash*leash;
}

void eSoldierAction::serializeFields(eSaveArchive& ar) {
    eFightingAction::serializeFields(ar);
    ar.field("spreadPeriod", mSpreadPeriod);
    ar.field("goToBannerCountdown", mGoToBannerCountdown, 0);
    ar.field("arrivedAtBanner", mArrivedAtBanner, false);
    ar.field("soldierStage", mStage, eSoldierActionStage::idle);
}

void eSoldierAction::resumeFromSavedState() {
    if(isAttacking()) {
        return eFightingAction::resumeFromSavedState();
    }
    rebuildCurrentStage();
}

void eSoldierAction::rebuildCurrentStage() {
    switch(mStage) {
    case eSoldierActionStage::home:
        return goHome();
    case eSoldierActionStage::abroad:
        return goAbroad();
    case eSoldierActionStage::banner: {
        const stdptr<eSoldierAction> tptr(this);
        const auto taskFinished = [tptr]() {
            if(!tptr) return;
            tptr->mGoToBannerCountdown = 100;
        };
        const auto taskFindFailed = [tptr]() {
            if(!tptr) return;
            tptr->mGoToBannerCountdown = 1000;
        };
        const auto s = static_cast<eSoldier*>(character());
        const auto b = s->banner();
        if(b) goBackToBanner(b->soldierOrientation(),
                             taskFindFailed, taskFinished);
        return;
    }
    case eSoldierActionStage::idle:
        return eFightingAction::resumeFromSavedState();
    }
}

stdsptr<eObsticleHandler> eSoldierAction::obsticleHandler() {
    return std::make_shared<eSoldierObsticleHandler>(
                board(), this);
}

void eSoldierAction::goHome() {
    mStage = eSoldierActionStage::home;
    mArrivedAtBanner = false;
    const auto c = character();
    c->setSpeed(52.5);
    const auto& brd = c->getBoard();
    const auto type = c->type();
    const auto cid = cityId();
    const auto b = sFindHome(type, cid, brd);
    if(!b) {
        c->kill();
        return;
    }

    const stdptr<eSoldierAction> tptr(this);
    const stdptr<eCharacter> cptr(c);
    const auto finishAct = std::make_shared<eSA_goHomeFinish>(
                               board(), c);

    const auto a = e::make_shared<eMoveToAction>(cptr.get());
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finishAct);
    a->setFinishAction(finishAct);
    a->setFoundAction([tptr, cptr]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
    });
    a->start(b, eWalkableObject::sCreateDefault());
    setCurrentAction(a);
}

void eSoldierAction::goAbroad() {
    mStage = eSoldierActionStage::abroad;
    const auto c = character();
    auto& board = eSoldierAction::board();
    const auto cid = onCityId();
    const auto hero = static_cast<eCharacter*>(c);
    const stdptr<eCharacter> cptr(hero);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
                          board, hero);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
                            board, hero);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);

    const auto exitPoint = board.exitPoint(cid);
    if(exitPoint) {
        a->start(exitPoint);
    } else {
        const auto edgeTile = [](eTileBase* const tile) {
            return tile->isCityEdge();
        };
        a->start(edgeTile);
    }
}

eBuilding* eSoldierAction::sFindHome(const eCharacterType t,
                                     const eCityId cid,
                                     const eGameBoard& brd) {
    eGameBoard::eBuildingValidator v;
    if(t == eCharacterType::rockThrower ||
       t == eCharacterType::archerPoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::commonHouse) return false;
            const auto ch = static_cast<eSmallHouse*>(b);
            if(ch->level() < 2) return false;
            return true;
        };
    } else if(t == eCharacterType::hoplite ||
              t == eCharacterType::hoplitePoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::eliteHousing) return false;
            const auto eh = static_cast<eEliteHousing*>(b);
            if(eh->level() < 2) return false;
            return true;
        };
     } else if(t == eCharacterType::horseman ||
               t == eCharacterType::chariotPoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::eliteHousing) return false;
            const auto eh = static_cast<eEliteHousing*>(b);
            if(eh->level() < 4) return false;
            return true;
        };
    } else if(t == eCharacterType::amazon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::temple) return false;
            const auto eh = static_cast<eTempleBuilding*>(b);
            if(!eh->finished()) return false;
            const auto s = static_cast<eSanctuary*>(eh->monument());
            if(s->godType() != eGodType::artemis) return false;
            return true;
        };
    } else if(t == eCharacterType::aresWarrior) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::temple) return false;
            const auto eh = static_cast<eTempleBuilding*>(b);
            if(!eh->finished()) return false;
            const auto s = static_cast<eSanctuary*>(eh->monument());
            if(s->godType() != eGodType::ares) return false;
            return true;
        };
    } else {
        return nullptr;
    }
    const auto b = brd.randomBuilding(cid, v);
    return b;
}

void eSoldierAction::goBackToBanner(const eOrientation facing,
                                    const eAction& findFailAct,
                                    const eAction& findFinishAct) {
    mStage = eSoldierActionStage::banner;
    const auto c = character();
    const auto s = static_cast<eSoldier*>(c);
    const auto b = s->banner();
    if(!b) return;

    const auto standAtBanner = [&]() {
        if(!mArrivedAtBanner) {
            mArrivedAtBanner = true;
            c->setSpeed(52.5);
        }
        setCurrentAction(nullptr);
        c->setOrientation(facing);
        c->setActionType(eCharacterActionType::stand);
    };

    const auto ct = c->tile();
    const auto tt = b->place(s);
    if(!tt) {
        standAtBanner();
        return;
    }
    if(ct == tt) {
        standAtBanner();
        return;
    }

    const int ttx = tt->x();
    const int tty = tt->y();

    const bool isPersonPlayer = board().cityIdToPlayerId(cityId()) == board().personPlayer();
    if(!mArrivedAtBanner && isPersonPlayer) c->setSpeed(105.0);
    const auto type = b->type();
    setOverwrittableAction(type == eBannerType::enemy);
    goTo(ttx, tty, 0, findFailAct, findFinishAct);
}
