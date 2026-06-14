#include "god-worshipped-action.h"
#include "fileIO/esavearchive.h"

#include "characters/actions/edefendcityaction.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/eheroaction.h"
#include "vec2.h"
#include "enumbers.h"

GodWorshippedAction::GodWorshippedAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::godWorshippedAction) {}

void GodWorshippedAction::increment(const int by) {
    const int blessPeriod = eNumbers::sGodWorshippedBlessPeriod;
    const int blessRange = eNumbers::sGodWorshippedBlessRange;
    const bool r = lookForTargetedBlessCurse(by, mLookForBless,
                                             blessPeriod, blessRange, 1);
    if(!r) {
        const int attackPeriod = eNumbers::sGodWorshippedSoldierAttackPeriod;
        const int attackRange = eNumbers::sGodWorshippedSoldierAttackRange;
        const bool r = lookForSoldierAttack(by, mLookForSoldierAttack,
                                            attackPeriod, attackRange);
        if(!r) {
            if(mStage != GodWorshippedStage::defend &&
               mStage != GodWorshippedStage::huntMonster &&
               mStage != GodWorshippedStage::fightMonster) {
                const auto t = type();
                if(t == GodType::apollo) {
                    const int lookForMonsterCheck = 10000;
                    mLookForMonster += by;
                    if(mLookForMonster > lookForMonsterCheck) {
                        mLookForMonster -= lookForMonsterCheck;
                        lookForMonster();
                    }
                }
                const int lookForCityDefenseCheck = 5000;
                mLookForCityDefense += by;
                if(mLookForCityDefense > lookForCityDefenseCheck) {
                    mLookForCityDefense -= lookForCityDefenseCheck;
                    defendCity();
                }
            }
        }
    }
    if(mStage == GodWorshippedStage::huntMonster) {
        lookForMonsterFight();
    }

    eGodAction::increment(by);
}

bool GodWorshippedAction::decide() {
    const auto c = character();
    switch(mStage) {
    case GodWorshippedStage::none:
        mStage = GodWorshippedStage::appear;
        if(!c->tile()) randomPlaceOnBoard();
        if(!c->tile()) {
            c->kill();
        } else {
            appear();
        }
        break;
    case GodWorshippedStage::defend:
    case GodWorshippedStage::huntMonster:
    case GodWorshippedStage::fightMonster:
    case GodWorshippedStage::appear:
        mStage = GodWorshippedStage::goTo1;
        goToTarget();
        break;
    case GodWorshippedStage::goTo1: {
        mStage = GodWorshippedStage::patrol1;
        const auto tile = c->tile();
        const int len = tile->roadLength(5);
        if(len >= 5) patrol();
        else moveAround();
    }   break;
    case GodWorshippedStage::patrol1:
        mStage = GodWorshippedStage::goTo2;
        goToTarget();
        break;
    case GodWorshippedStage::goTo2: {
        mStage = GodWorshippedStage::patrol2;
        const auto tile = c->tile();
        const int len = tile->roadLength(5);
        if(len >= 5) patrol();
        else moveAround();
    }   break;
    case GodWorshippedStage::patrol2:
        mStage = GodWorshippedStage::disappear;
        disappear();
        break;
    case GodWorshippedStage::disappear:
        c->kill();
        break;
    }
    return true;
}

void GodWorshippedAction::lookForMonster() {
    const auto& board = GodWorshippedAction::board();
    const auto& ms = board.monsters(onCityId());
    for(const auto m : ms) {
        if(m->dead()) continue;
        huntMonster(m, false);
    }
}

void GodWorshippedAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void GodWorshippedAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case GodWorshippedStage::none:
    case GodWorshippedStage::appear:
    case GodWorshippedStage::goTo1:
    case GodWorshippedStage::patrol1:
    case GodWorshippedStage::goTo2:
    case GodWorshippedStage::patrol2:
    case GodWorshippedStage::disappear:
    case GodWorshippedStage::defend:
    case GodWorshippedStage::huntMonster:
    case GodWorshippedStage::fightMonster:
        eGodAction::resumeFromSavedState();
        return;
    }
}

void GodWorshippedAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("lookForBless", mLookForBless);
    ar.field("lookForSoldierAttack", mLookForSoldierAttack);
    ar.field("lookForCityDefense", mLookForCityDefense);
    ar.field("lookForMonster", mLookForMonster);
}

void GodWorshippedAction::defendCity() {
    auto& board = GodWorshippedAction::board();
    const auto cid = cityId();
    const auto i = board.invasionToDefend(cid);
    if(!i) return;
    mStage = GodWorshippedStage::defend;
    const auto c = character();
    const auto da = e::make_shared<eDefendCityAction>(c);
    setCurrentAction(da);
}

void GodWorshippedAction::lookForMonsterFight() {
    const auto c = character();
    const auto ct = c->tile();
    if(!ct) return;
    const int tx = ct->x();
    const int ty = ct->y();
    auto& board = c->getBoard();
    const auto tid = c->teamId();
    const int range = 5;
    for(int i = -range; i <= range; i++) {
        for(int j = -range; j <= range; j++) {
            const auto t = board.tile(tx + i, ty + j);
            if(!t) continue;
            const auto& chars = t->characters();
            for(const auto& cc : chars) {
                if(cc->dead()) continue;
                const auto cctype = cc->type();
                bool monster;
                eMonster::sCharacterToMonsterType(cctype, &monster);
                if(!monster) continue;
                const auto cctid = cc->teamId();
                if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                const auto m = static_cast<eMonster*>(cc.get());
                const bool r = fightMonster(m);
                if(r) return;
            }
        }
    }
}

bool GodWorshippedAction::fightMonster(eMonster* const m) {
    const auto c = character();
    const vec2d cpos{c->absX(), c->absY()};
    const vec2d mpos{m->absX(), m->absY()};
    const vec2d posdif = mpos - cpos;
    const double dist = posdif.length();
    const double range = 5.;
    if(dist > range) return false;
    const auto angle = posdif.angle();
    const auto o = sAngleOrientation(angle);
    c->setOrientation(o);
    c->setActionType(eCharacterActionType::fight);
    stdsptr<eCharacterAction> ca;
    const int fightTime = 5000;
    const int attackTime = God::sGodAttackTime(type());
    const auto gm = e::make_shared<GodMonsterActionInd>(c);
    gm->spawnTimedMissiles(eCharacterActionType::fight,
                           c->type(), attackTime, m,
                           nullptr, nullptr, nullptr,
                           fightTime);
    ca = gm;
    const auto mdie = std::make_shared<eHA_fightMonsterDie>(
                          board(), m);
    ca->setFailAction(mdie);
    ca->setFinishAction(mdie);
    setCurrentAction(ca);

    mStage = GodWorshippedStage::fightMonster;

    return true;
}

void GodWorshippedAction::huntMonster(eMonster* const m, const bool second) {
    const auto mt = m->tile();
    if(!mt) return;
    const auto mtype = m->type();

    const auto c = character();

    const auto finish = std::make_shared<eGWA_huntMonsterFinish>(
                            board(), this);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    const stdptr<GodWorshippedAction> tptr(this);
    const stdptr<eMonster> mptr(m);
    a->setFoundAction([tptr, mptr, this, a, c, second]() {
        if(!tptr || !mptr) return;
        if(second) {
            mStage = GodWorshippedStage::huntMonster;
            c->setActionType(eCharacterActionType::walk);
        } else {
            huntMonster(mptr, true);
        }
    });
    a->setRemoveLastTurn(true);
    a->setWait(false);
    if(mtype == eCharacterType::scylla ||
       mtype == eCharacterType::kraken) {
        a->start(mt, WalkableObject::sCreateWaterAndDefault());
    } else {
        a->start(mt, WalkableObject::sCreateDefault());
    }
    if(second) {
        setCurrentAction(a);
    }
}
