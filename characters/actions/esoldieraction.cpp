#include "esoldieraction.h"

#include "characters/esoldier.h"
#include "engine/egameboard.h"

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

    const auto r = lookForEnemy(by);

    if(r != eLookForEnemyState::none) return;

    if(!currentAction()) {
        mGoToBannerCountdown -= by;
        if(mGoToBannerCountdown < 0) {
            mGoToBannerCountdown = 1000;
            const stdptr<eSoldierAction> tptr(this);
            const auto taskFinished = [tptr]() {
                if(!tptr) return;
                tptr->mGoToBannerCountdown = 500;
            };
            const auto taskFindFailed = [tptr]() {
                if(!tptr) return;
                tptr->mGoToBannerCountdown = 5000;
            };

            goBackToBanner(taskFindFailed, taskFinished);
        }
    }

    eComplexAction::increment(by);
}

void eSoldierAction::read(eReadStream& src) {
    eFightingAction::read(src);
    src >> mSpreadPeriod;
}

void eSoldierAction::write(eWriteStream& dst) const {
    eFightingAction::write(dst);
    dst << mSpreadPeriod;
}

stdsptr<eObsticleHandler> eSoldierAction::obsticleHandler() {
    return std::make_shared<eSoldierObsticleHandler>(
                board(), this);
}

void eSoldierAction::goHome() {
    const auto c = character();
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
    const auto c = character();
    auto& board = eSoldierAction::board();
    const auto cid = cityId();
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

void eSoldierAction::goBackToBanner(const eAction& findFailAct,
                                    const eAction& findFinishAct) {
    const auto c = character();
    const auto s = static_cast<eSoldier*>(c);
    const auto b = s->banner();
    if(!b) return;

    const auto ct = c->tile();
    const auto tt = b->place(s);
    if(!tt) return;
    if(ct == tt) {
        setCurrentAction(nullptr);
        c->setActionType(eCharacterActionType::stand);
        return;
    }

    const int ttx = tt->x();
    const int tty = tt->y();

    const auto type = b->type();
    setOverwrittableAction(type == eBannerType::enemy);
    goTo(ttx, tty, 0, findFailAct, findFinishAct);
}
