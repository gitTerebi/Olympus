#include "soldier-action.h"
#include "fileIO/save-archive.h"

#include "characters/esoldier.h"
#include "engine/game-board.h"
#include "engine/etile.h"

#include <cstdio>
#include <math.h>

#include "characters/soldier-banner.h"

#include "emovetoaction.h"
#include "buildings/small-house.h"
#include "buildings/ebuilding.h"
#include "buildings/elite-housing.h"
#include "buildings/sanctuaries/etemplebuilding.h"
#include "ekillcharacterfinishfail.h"
#include "numbers.h"

SoldierAction::SoldierAction(eCharacter *const c) : FightingAction(c, eCharActionType::soldierAction) {}

bool SoldierObsticleHandler::handle(eTile *const tile)
{
    if (!mTptr)
        return false;
    const auto ub = tile->underBuilding();
    if (!ub)
        return false;
    const auto ubt = ub->type();
    const bool r = eBuilding::sWalkableBuilding(ubt);
    if (r)
        return false;
    mTptr->setCombatBlockage(ub);
    mTptr->attackBuilding(tile, false);
    return true;
}

bool SoldierAction::decide()
{
    return true;
}

eTile *SoldierAction::repositionAnchor() const
{
    const auto c = character();
    const auto s = static_cast<eSoldier *>(c);
    const auto b = s->banner();
    if (!b)
        return c->tile();
    const auto slot = b->place(s);
    // Ranged soldiers fire from their formation slot, stepping out at most a
    // couple tiles to a tile that can shoot. Fall back to the current tile if
    // no slot is assigned.
    return slot ? slot : c->tile();
}

void SoldierAction::increment(const int by)
{
    if (mSpreadPeriod && currentAction())
    {
        return eComplexAction::increment(by);
    }
    else
    {
        mSpreadPeriod = false;
    }

    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    const bool bannerEnemy = b && b->type() == eBannerType::enemy;
    const bool enNear = bannerEnemy ? false : (b ? b->enemyNear(by) : false);

    mFollowDirectorCooldown -= by;
    if (mFollowDirectorCooldown <= 0)
    {
        mFollowDirectorCooldown = 500;
        if (followBannerDirector())
        {
            return eComplexAction::increment(by);
        }
    }
    else if (mStage == SoldierActionStage::chase)
    {
        return eComplexAction::increment(by);
    }

    if (mStage == SoldierActionStage::banner && currentAction() &&
        !isAttacking() && (bannerEnemy || !enNear))
    {
        return eComplexAction::increment(by);
    }

    // Player ordered retreat: goHome/goAbroad set the walk and the stage. Let
    // that walk run even with enemies near — otherwise lookForEnemy below short
    // circuits every tick and the soldier stands and fights instead of obeying
    // the dismiss-to-palace command. Drop any live attack first so the walk owns
    // the soldier.
    if (mStage == SoldierActionStage::abroad && !currentAction() && mDepartDelay > 0)
    {
        mDepartDelay -= by;
        if (mDepartDelay > 0)
            return;
        mDepartDelay = 0;
        goAbroad();
        return;
    }

    if ((mStage == SoldierActionStage::home ||
         mStage == SoldierActionStage::abroad) &&
        currentAction())
    {
        if (isAttacking())
            cancelAttack();
        return eComplexAction::increment(by);
    }

    // Idle: no enemy near the formation and not mid-attack -> skip the combat
    // scan entirely, just run the cheap banner-return tick. Exception: if the
    // banner is parked ON an enemy building (general pinned it there to attack),
    // directly attack the building tile so soldiers grind it from adjacent slots.
    if (!isAttacking() && !enNear && !bannerEnemy)
    {
        if (b && !currentAction())
        {
            const auto bt = b->tile();
            if (bt)
            {
                const auto ub = bt->underBuilding();
                const auto tid = character()->teamId();
                if (ub && eBuilding::sAttackable(ub->type()) &&
                    eTeamIdHelpers::isEnemy(ub->teamId(), tid))
                {
                    attackBuilding(bt, false);
                    return eComplexAction::increment(by);
                }
            }
        }
        tickBannerReturn(by);
        return eComplexAction::increment(by);
    }

    // Don't yank a soldier back to its banner while a fight is live — that let
    // a player lure one invader past the leash, snap it home, and pick off the
    // formation piecemeal. Hold and finish the fight; the leash only reins the
    // soldier in once no enemy is near.
    const auto r = lookForEnemy(by);

    if (r != LookForEnemyState::none)
        return;

    tickBannerReturn(by);

    eComplexAction::increment(by);
}

bool SoldierAction::followBannerDirector()
{
    const auto c = character();
    const auto s = static_cast<eSoldier *>(c);
    const auto b = s->banner();
    if (!b)
        return false;

    SoldierBanner::CombatAssignment a;
    const bool hasAssignment = b->combatAssignment(s, a);

    const auto exitChase = [&]()
    {
        if (mStage == SoldierActionStage::chase)
        {
            mStage = SoldierActionStage::idle;
            mChaseTarget = nullptr;
            if (!isAttacking())
            {
                setCurrentAction(nullptr);
                setOverwrittableAction(true);
            }
        }
    };

    if (!hasAssignment || (a.target && a.target->dead()))
    {
        exitChase();
        return false;
    }
    if (isAttacking())
        return false;
    if (currentAction() && !overwrittableAction())
        return true;
    const auto cat = c->actionType();
    if (cat == eCharacterActionType::fight ||
        cat == eCharacterActionType::fight2)
        return false;

    const auto ct = c->tile();
    if (!ct)
        return false;

    // If an enemy is already adjacent, don't issue a new walk — let the
    // adjacency scan in lookForEnemy lock on and fight.
    if (c->range() == 0)
    {
        auto &brd = c->getBoard();
        const auto tid = c->teamId();
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (!i && !j)
                    continue;
                const auto t = brd.tile(ct->x() + i, ct->y() + j);
                if (!t)
                    continue;
                for (const auto &cc : t->characters())
                {
                    if (cc->dead())
                        continue;
                    if (!eTeamIdHelpers::isEnemy(cc->teamId(), tid))
                        continue;
                    if (!cc->isSoldier() && !cc->isImmortal())
                        continue;
                    exitChase();
                    return false;
                }
            }
        }
    }

    if (a.intent == SoldierBanner::CombatAssignment::Intent::clearObstacle)
    {
        if (c->range() > 0)
            return false;
        if (!a.targetBuilding)
            return false;
        const auto bt = a.targetBuilding->centerTile();
        if (!bt)
            return false;
        exitChase();
        setCurrentAction(nullptr);
        attackBuilding(bt, false);
        return true;
    }

    if (!a.standTile)
    {
        exitChase();
        return false;
    }

    if (ct == a.standTile)
    {
        exitChase();
        return false;
    }

    // Already chasing THIS exact tile — let the walk continue.
    if (mStage == SoldierActionStage::chase &&
        currentAction() && !overwrittableAction() &&
        mChaseTarget == a.standTile)
    {
        return true;
    }

    // Enter chase state and issue the walk.
    mStage = SoldierActionStage::chase;
    mChaseTarget = a.standTile;
    setOverwrittableAction(false);
    goTo(a.standTile->x(), a.standTile->y(), 0);
    return true;
}

void SoldierAction::setCombatBlockage(eBuilding *const b)
{
    const auto s = static_cast<eSoldier *>(character());
    const auto banner = s->banner();
    if (!banner)
        return;
    banner->setCombatBlockage(s, b);
}

void SoldierAction::tickBannerReturn(const int by)
{
    // The idle re-form: when a soldier sits with no action, a countdown ticks
    // and eventually walks it back to its formation slot. This is the "banner
    // pull". It must NOT fire during combat — in the gap between combat scans
    // lookForEnemy returns none, and an unguarded pull would drag a ranged unit
    // off its firing tile, giving the walk-forward-walk-back loop. enemyNear()
    // gates it: hold while any enemy is in the ranged detect box.
    if (currentAction())
        return;
    const auto s2 = static_cast<eSoldier *>(character());
    const auto b2 = s2->banner();
    if (b2 && b2->type() == eBannerType::enemy)
    {
        SoldierBanner::CombatAssignment a;
        if (b2->combatAssignment(s2, a))
            return; // has a job, don't pull back
    }
    else if (enemyNear())
        return;

    mGoToBannerCountdown -= by;
    if (mGoToBannerCountdown >= 0)
        return;
    mGoToBannerCountdown = 250;

    const stdptr<SoldierAction> tptr(this);
    const auto taskFinished = [tptr]()
    {
        if (!tptr)
            return;
        tptr->mGoToBannerCountdown = 100;
    };
    const auto taskFindFailed = [tptr]()
    {
        if (!tptr)
            return;
        tptr->mGoToBannerCountdown = 1000;
        const auto c = tptr->character();
        const auto ct = c ? c->tile() : nullptr;
        if (ct == tptr->mLastFailTile)
        {
            tptr->mPathFailCount++;
        }
        else
        {
            tptr->mPathFailCount = 1;
            tptr->mLastFailTile = ct;
        }
        if (tptr->mPathFailCount >= 5)
        {
            printf("[invasion-soldier] stuck x%d soldier=%p at=%d,%d\n",
                   tptr->mPathFailCount, (void *)c,
                   ct ? ct->x() : -1, ct ? ct->y() : -1);

            const auto s = static_cast<eSoldier *>(c);
            const auto b = s ? s->banner() : nullptr;
            if (b)
            {
                const auto slot = b->place(s);
                const auto dest = slot ? slot : b->tile();
                if (dest)
                    c->changeTile(dest);
            }
            tptr->mPathFailCount = 0;
            tptr->mLastFailTile = nullptr;
        }
    };

    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    if (b)
    {
        // Already at formation slot: stand, no walk needed.
        const auto slot = b->place(s);
        if (slot && character()->tile() == slot)
            return;
        // No slot: fall back to banner tile. If already there, nothing to do.
        if (!slot && character()->tile() == b->tile())
            return;
        // No slot and banner is pinned on an enemy building: don't walk toward
        // the unwalkable building tile. Let the idle attack path in increment()
        // fire attackBuilding directly on the next tick instead.
        if (!slot && b->type() == eBannerType::enemy)
        {
            const auto bt = b->tile();
            if (bt)
            {
                const auto ub = bt->underBuilding();
                const auto tid = character()->teamId();
                if (ub && eBuilding::sAttackable(ub->type()) &&
                    eTeamIdHelpers::isEnemy(ub->teamId(), tid))
                {
                    return;
                }
            }
        }
        goBackToBanner(b->soldierOrientation(),
                       taskFindFailed, taskFinished);
    }
}

bool SoldierAction::enemyNear() const
{
    // Delegate to the banner's shared cache (ticked in increment). Reading with
    // by=0 just returns the cached result — no extra scan.
    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    if (!b)
        return false;
    return b->enemyNear(0);
}

void SoldierAction::serializeFields(SaveArchive &ar)
{
    FightingAction::serializeFields(ar);
    ar.field("spreadPeriod", mSpreadPeriod);
    ar.field("goToBannerCountdown", mGoToBannerCountdown, 0);
    ar.field("followDirectorCooldown", mFollowDirectorCooldown, 0);
    ar.field("arrivedAtBanner", mArrivedAtBanner, false);
    ar.field("soldierStage", mStage, SoldierActionStage::idle);
}

void SoldierAction::resumeFromSavedState()
{
    if (isAttacking())
    {
        return FightingAction::resumeFromSavedState();
    }
    rebuildCurrentStage();
}

void SoldierAction::rebuildCurrentStage()
{
    switch (mStage)
    {
    case SoldierActionStage::home:
        return goHome();
    case SoldierActionStage::abroad:
        return goAbroad();
    case SoldierActionStage::banner:
    {
        const stdptr<SoldierAction> tptr(this);
        const auto taskFinished = [tptr]()
        {
            if (!tptr)
                return;
            tptr->mGoToBannerCountdown = 100;
        };
        const auto taskFindFailed = [tptr]()
        {
            if (!tptr)
                return;
            tptr->mGoToBannerCountdown = 1000;
        };
        const auto s = static_cast<eSoldier *>(character());
        const auto b = s->banner();
        if (b)
            goBackToBanner(b->soldierOrientation(),
                           taskFindFailed, taskFinished);
        return;
    }
    case SoldierActionStage::chase:
        mStage = SoldierActionStage::idle;
        mChaseTarget = nullptr;
        return FightingAction::resumeFromSavedState();
    case SoldierActionStage::idle:
        return FightingAction::resumeFromSavedState();
    }
}

bool SoldierAction::allowsSelfPositioning() const
{
    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    if (!b || b->type() != eBannerType::enemy)
        return true;
    return character()->range() > 0; // ranged invaders reposition; melee hold slot
}

bool SoldierAction::prefersPathAround() const
{
    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    return b && b->type() == eBannerType::enemy;
}

stdsptr<eObsticleHandler> SoldierAction::obsticleHandler()
{
    return std::make_shared<SoldierObsticleHandler>(
        board(), this);
}

void SoldierAction::beingAttacked(int ttx, int tty)
{
    // Enemy-banner soldiers do NOT individually charge whoever pinged them. When
    // one missile hits the banner, the old per-soldier response sent every man
    // beelining to the single attacker tile — 8 soldiers stacked on one spot,
    // formation gone. Instead let the BANNER brain (updateCombat) walk the whole
    // block toward the foe in formation; the soldiers hold their slots, the line
    // bumps into the enemy, and the reactive adjacency scan starts the fight. So
    // swallow the signal here: only matters before contact, and a soldier already
    // adjacent engages via lookForEnemy regardless.
    const auto s = static_cast<eSoldier *>(character());
    const auto b = s->banner();
    if (b && b->type() == eBannerType::enemy)
    {
        b->noteAttackFrom(ttx, tty);
        return;
    }
    FightingAction::beingAttacked(ttx, tty);
}

void SoldierAction::goHome()
{
    mStage = SoldierActionStage::home;
    mArrivedAtBanner = false;
    const auto c = character();
    c->setSpeed(52.5);
    const auto &brd = c->getBoard();
    const auto type = c->type();
    const auto cid = cityId();
    const auto b = sFindHome(type, cid, brd);
    if (!b)
    {
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
    a->setFoundAction([tptr, cptr]()
                      {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk); });
    a->start(b, WalkableObject::sCreateDefault());
    setCurrentAction(a);
}

void SoldierAction::goAbroad()
{
    mStage = SoldierActionStage::abroad;
    if (mDepartDelay > 0)
        return;
    const auto c = character();
    auto &board = SoldierAction::board();
    const auto cid = onCityId();
    const auto hero = static_cast<eCharacter *>(c);
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
    a->setFindFailAction([cptr]()
                         {
        if(cptr) cptr->kill(); });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);

    const auto edgeTile = [](eTileBase *const tile)
    {
        return tile->isCityEdge();
    };
    a->start(edgeTile);
}

eBuilding *SoldierAction::sFindHome(const eCharacterType t,
                                    const eCityId cid,
                                    const GameBoard &brd)
{
    GameBoard::eBuildingValidator v;
    if (t == eCharacterType::rockThrower ||
        t == eCharacterType::archerPoseidon)
    {
        v = [](eBuilding *const b)
        {
            const auto bt = b->type();
            if (bt != eBuildingType::commonHouse)
                return false;
            const auto ch = static_cast<SmallHouse *>(b);
            if (ch->level() < 2)
                return false;
            return true;
        };
    }
    else if (t == eCharacterType::hoplite ||
             t == eCharacterType::hoplitePoseidon)
    {
        v = [](eBuilding *const b)
        {
            const auto bt = b->type();
            if (bt != eBuildingType::eliteHousing)
                return false;
            const auto eh = static_cast<EliteHousing *>(b);
            if (eh->level() < 1)
                return false;
            return true;
        };
    }
    else if (t == eCharacterType::horseman ||
             t == eCharacterType::chariotPoseidon)
    {
        v = [](eBuilding *const b)
        {
            const auto bt = b->type();
            if (bt != eBuildingType::eliteHousing)
                return false;
            const auto eh = static_cast<EliteHousing *>(b);
            if (eh->level() < 3)
                return false;
            return true;
        };
    }
    else if (t == eCharacterType::amazon)
    {
        v = [](eBuilding *const b)
        {
            const auto bt = b->type();
            if (bt != eBuildingType::temple)
                return false;
            const auto eh = static_cast<eTempleBuilding *>(b);
            if (!eh->finished())
                return false;
            const auto s = static_cast<eSanctuary *>(eh->monument());
            if (s->godType() != GodType::artemis)
                return false;
            return true;
        };
    }
    else if (t == eCharacterType::aresWarrior)
    {
        v = [](eBuilding *const b)
        {
            const auto bt = b->type();
            if (bt != eBuildingType::temple)
                return false;
            const auto eh = static_cast<eTempleBuilding *>(b);
            if (!eh->finished())
                return false;
            const auto s = static_cast<eSanctuary *>(eh->monument());
            if (s->godType() != GodType::ares)
                return false;
            return true;
        };
    }
    else
    {
        return nullptr;
    }
    const auto b = brd.randomBuilding(cid, v);
    return b;
}

void SoldierAction::goBackToBanner(const eOrientation facing,
                                   const eAction &findFailAct,
                                   const eAction &findFinishAct)
{
    cancelAndClearAction();
    mStage = SoldierActionStage::banner;
    const auto c = character();
    const auto s = static_cast<eSoldier *>(c);
    const auto b = s->banner();
    if (!b)
        return;

    const auto standAtBanner = [&]()
    {
        if (!mArrivedAtBanner)
        {
            mArrivedAtBanner = true;
            c->setSpeed(52.5);
        }
        setCurrentAction(nullptr);
        c->setOrientation(facing);
        c->setActionType(eCharacterActionType::stand);
    };

    const auto ct = c->tile();
    const auto tt = b->place(s) ? b->place(s) : b->tile();
    if (!tt || ct == tt)
    {
        standAtBanner();
        return;
    }

    const int ttx = tt->x();
    const int tty = tt->y();

    if (b->type() == eBannerType::enemy)
    {
        setOverwrittableAction(true);
        c->setActionType(eCharacterActionType::walk);
    }
    else
    {
        const bool isPersonPlayer = board().cityIdToPlayerId(cityId()) == board().personPlayer();
        if (!mArrivedAtBanner && isPersonPlayer)
            c->setSpeed(105.0);
        setOverwrittableAction(false);
    }
    goTo(ttx, tty, 0, findFailAct, findFinishAct);
}
