#include "soldier-action.h"
#include "fileIO/esavearchive.h"

#include "characters/esoldier.h"
#include "engine/e-game-board.h"

#include <math.h>

#include "characters/soldier-banner.h"

#include "emovetoaction.h"
#include "buildings/small-house.h"
#include "buildings/elite-housing.h"
#include "buildings/sanctuaries/etemplebuilding.h"
#include "ekillcharacterfinishfail.h"

SoldierAction::SoldierAction(eCharacter* const c) :
    FightingAction(c, eCharActionType::soldierAction) {}

bool SoldierAction::decide() {
    return true;
}

eTile* SoldierAction::repositionAnchor() const {
    const auto c = character();
    const auto s = static_cast<eSoldier*>(c);
    const auto b = s->banner();
    if(!b) return c->tile();
    const auto slot = b->place(s);
    // Ranged soldiers fire from their formation slot, stepping out at most a
    // couple tiles to a tile that can shoot. Fall back to the current tile if
    // no slot is assigned.
    return slot ? slot : c->tile();
}

void SoldierAction::increment(const int by) {
    if(mSpreadPeriod && currentAction()) {
        return eComplexAction::increment(by);
    } else {
        mSpreadPeriod = false;
    }

    // Marching to the banner slot: let the walk run UNLESS an enemy is near.
    // Without the enemyNear gate a soldier walks straight past a free adjacent
    // foe to reach its slot and never engages — the "enemy next and free but
    // just stands there" bug. With an enemy near, fall through to lookForEnemy
    // so it breaks off and fights.
    if(followBannerDirector()) {
        return eComplexAction::increment(by);
    }

    const auto s = static_cast<eSoldier*>(character());
    const auto b = s->banner();
    const bool bannerDirected = b && b->type() == eBannerType::enemy;
    if(mStage == SoldierActionStage::banner && currentAction() &&
       (bannerDirected || !enemyNear())) {
        return eComplexAction::increment(by);
    }

    // Player ordered retreat: goHome/goAbroad set the walk and the stage. Let
    // that walk run even with enemies near — otherwise lookForEnemy below short
    // circuits every tick and the soldier stands and fights instead of obeying
    // the dismiss-to-palace command. Drop any live attack first so the walk owns
    // the soldier.
    if((mStage == SoldierActionStage::home ||
        mStage == SoldierActionStage::abroad) && currentAction()) {
        if(isAttacking()) cancelAttack();
        return eComplexAction::increment(by);
    }

    // Routed banner: its soldiers break off and flee the map instead of fighting
    // to the last man (Augustus morale rout). Drop any attack (releases the
    // combat claim) and head abroad. Once fleeing (stage abroad) just let the
    // walk run.
    {
        if(b && b->routed()) {
            if(mStage != SoldierActionStage::abroad) {
                if(isAttacking()) cancelAttack();
                goAbroad();
            }
            return eComplexAction::increment(by);
        }
    }

    // Don't yank a soldier back to its banner while a fight is live — that let
    // a player lure one invader past the leash, snap it home, and pick off the
    // formation piecemeal. Hold and finish the fight; the leash only reins the
    // soldier in once no enemy is near.
    const auto r = lookForEnemy(by);

    if(r != LookForEnemyState::none) return;

    tickBannerReturn(by);

    eComplexAction::increment(by);
}

bool SoldierAction::followBannerDirector() {
    const auto c = character();
    const auto s = static_cast<eSoldier*>(c);
    const auto b = s->banner();
    if(!b) return false;
    SoldierBanner::CombatAssignment a;
    if(!b->combatAssignment(s, a)) return false;
    if(a.target && a.target->dead()) return false;
    if(!a.standTile) return false;
    if(c->range() > 0) return false;
    if(isAttacking()) return false;

    const auto ct = c->tile();
    if(!ct) return false;
    if(ct == a.standTile) {
        setCurrentAction(nullptr);
        setOverwrittableAction(true);
        return false;
    }
    if(currentAction() && !overwrittableAction()) return true;

    setOverwrittableAction(false);
    goTo(a.standTile->x(), a.standTile->y(), 0);
    return true;
}

void SoldierAction::tickBannerReturn(const int by) {
    // The idle re-form: when a soldier sits with no action, a countdown ticks
    // and eventually walks it back to its formation slot. This is the "banner
    // pull". It must NOT fire during combat — in the gap between combat scans
    // lookForEnemy returns none, and an unguarded pull would drag a ranged unit
    // off its firing tile, giving the walk-forward-walk-back loop. enemyNear()
    // gates it: hold while any enemy is in the ranged detect box.
    if(currentAction() || enemyNear()) return;

    mGoToBannerCountdown -= by;
    if(mGoToBannerCountdown >= 0) return;
    mGoToBannerCountdown = 250;

    const stdptr<SoldierAction> tptr(this);
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
    if(b) {
        goBackToBanner(b->soldierOrientation(),
                       taskFindFailed, taskFinished);
    }
}

bool SoldierAction::enemyNear() const {
    const auto c = character();
    const auto ct = c->tile();
    if(!ct) return false;
    const int tx = ct->x();
    const int ty = ct->y();
    const auto tid = c->teamId();
    auto& brd = c->getBoard();
    // Wide awareness: hold position while any enemy is within the ranged detect
    // box so the unit waits for it to close rather than being yanked home
    // between combat scans. Shares the constant with the reposition scan.
    const int hrange = FightingAction::sRangedDetectRange(c->range());
    for(int i = -hrange; i <= hrange; i++) {
        for(int j = -hrange; j <= hrange; j++) {
            const auto t = brd.tile(tx + i, ty + j);
            if(!t) continue;
            for(const auto& cc : t->characters()) {
                if(cc->dead()) continue;
                if(!eTeamIdHelpers::isEnemy(cc->teamId(), tid)) continue;
                if(!cc->isSoldier() && cc->type() != eCharacterType::wolf &&
                   !cc->isImmortal()) continue;
                return true;
            }
        }
    }
    return false;
}

void SoldierAction::serializeFields(eSaveArchive& ar) {
    FightingAction::serializeFields(ar);
    ar.field("spreadPeriod", mSpreadPeriod);
    ar.field("goToBannerCountdown", mGoToBannerCountdown, 0);
    ar.field("arrivedAtBanner", mArrivedAtBanner, false);
    ar.field("soldierStage", mStage, SoldierActionStage::idle);
}

void SoldierAction::resumeFromSavedState() {
    if(isAttacking()) {
        return FightingAction::resumeFromSavedState();
    }
    rebuildCurrentStage();
}

void SoldierAction::rebuildCurrentStage() {
    switch(mStage) {
    case SoldierActionStage::home:
        return goHome();
    case SoldierActionStage::abroad:
        return goAbroad();
    case SoldierActionStage::banner: {
        const stdptr<SoldierAction> tptr(this);
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
    case SoldierActionStage::idle:
        return FightingAction::resumeFromSavedState();
    }
}

stdsptr<eObsticleHandler> SoldierAction::obsticleHandler() {
    return std::make_shared<SoldierObsticleHandler>(
                board(), this);
}

void SoldierAction::beingAttacked(int ttx, int tty) {
    // Enemy-banner soldiers do NOT individually charge whoever pinged them. When
    // one missile hits the banner, the old per-soldier response sent every man
    // beelining to the single attacker tile — 8 soldiers stacked on one spot,
    // formation gone. Instead let the BANNER brain (updateCombat) walk the whole
    // block toward the foe in formation; the soldiers hold their slots, the line
    // bumps into the enemy, and the reactive adjacency scan starts the fight. So
    // swallow the signal here: only matters before contact, and a soldier already
    // adjacent engages via lookForEnemy regardless.
    const auto s = static_cast<eSoldier*>(character());
    const auto b = s->banner();
    if(b && b->type() == eBannerType::enemy && !isAttacking()) return;
    FightingAction::beingAttacked(ttx, tty);
}

void SoldierAction::goHome() {
    mStage = SoldierActionStage::home;
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

    const stdptr<SoldierAction> tptr(this);
    const stdptr<eCharacter> cptr(c);
    const auto finishAct = std::make_shared<SA_goHomeFinish>(
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

void SoldierAction::goAbroad() {
    mStage = SoldierActionStage::abroad;
    const auto c = character();
    auto& board = SoldierAction::board();
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

eBuilding* SoldierAction::sFindHome(const eCharacterType t,
                                     const eCityId cid,
                                     const GameBoard& brd) {
    GameBoard::eBuildingValidator v;
    if(t == eCharacterType::rockThrower ||
       t == eCharacterType::archerPoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::commonHouse) return false;
            const auto ch = static_cast<SmallHouse*>(b);
            if(ch->level() < 2) return false;
            return true;
        };
    } else if(t == eCharacterType::hoplite ||
              t == eCharacterType::hoplitePoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::eliteHousing) return false;
            const auto eh = static_cast<EliteHousing*>(b);
            if(eh->level() < 2) return false;
            return true;
        };
     } else if(t == eCharacterType::horseman ||
               t == eCharacterType::chariotPoseidon) {
        v = [](eBuilding* const b) {
            const auto bt = b->type();
            if(bt != eBuildingType::eliteHousing) return false;
            const auto eh = static_cast<EliteHousing*>(b);
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

void SoldierAction::goBackToBanner(const eOrientation facing,
                                    const eAction& findFailAct,
                                    const eAction& findFinishAct) {
    mStage = SoldierActionStage::banner;
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
