#include "god-minion-action.h"

#include "fileIO/esavearchive.h"

#include "characters/monsters/emonster.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/eheroaction.h"
#include "characters/gods/actions/god-action.h"
#include "characters/actions/ewaitaction.h"
#include "erand.h"
#include "engine/game-board.h"
#include "vec2.h"

eGodMinionAction::eGodMinionAction(eCharacter* const c) :
    eGodMonsterAction(c, eCharActionType::godMinionAction) {}

void eGodMinionAction::increment(const int by) {
//    if(mStage != eGodMinionStage::huntMonster &&
//       mStage != eGodMinionStage::fightMonster) {
//        const int lookForMonsterCheck = 5000;
//        mLookForMonster += by;
//        if(mLookForMonster > lookForMonsterCheck) {
//            mLookForMonster -= lookForMonsterCheck;
//            lookForMonster();
//        }
//    }
//    if(mStage == eGodMinionStage::huntMonster) {
//        lookForMonsterFight();
//    }

    if(mStage != eGodMinionStage::huntSoldier &&
       mStage != eGodMinionStage::fightSoldier) {
        const int lookForSoldierCheck = 5000;
        mLookForSoldier += by;
        if(mLookForSoldier > lookForSoldierCheck) {
            mLookForSoldier -= lookForSoldierCheck;
            lookForSoldier();
        }
    }
    if(mStage == eGodMinionStage::huntSoldier) {
        const int lookForFightCheck = 250;
        mApproachCheck += by;
        if(mApproachCheck > lookForFightCheck) {
            mApproachCheck -= lookForFightCheck;
            lookForSoldierFight();
        }
    }

    eGodMonsterAction::increment(by);
}

bool eGodMinionAction::decide() {
    const auto c = character();
    switch(mStage) {
    case eGodMinionStage::none:
        mStage = eGodMinionStage::appear;
        if(!c->tile()) randomPlaceOnBoard();
        if(!c->tile()) {
            c->kill();
        } else {
            appear();
        }
        break;
    case eGodMinionStage::huntMonster:
    case eGodMinionStage::fightMonster:
    case eGodMinionStage::huntSoldier:
    case eGodMinionStage::fightSoldier:
    case eGodMinionStage::appear:
        mStage = eGodMinionStage::goTo1;
        goToTarget(eHeatGetters::any, nullptr);
        break;
    case eGodMinionStage::goTo1: {
        mStage = eGodMinionStage::patrol1;
        const auto tile = c->tile();
        const int len = tile ? tile->roadLength(5) : 0;
        if(len >= 5) patrol();
        else moveAround();
    }   break;
    case eGodMinionStage::patrol1:
        mStage = eGodMinionStage::goTo2;
        goToTarget(eHeatGetters::any, nullptr);
        break;
    case eGodMinionStage::goTo2: {
        mStage = eGodMinionStage::patrol2;
        const auto tile = c->tile();
        const int len = tile ? tile->roadLength(5) : 0;
        if(len >= 5) patrol();
        else moveAround();
    }   break;
    case eGodMinionStage::patrol2:
        mStage = eGodMinionStage::disappear;
        disappear(true);
        break;
    case eGodMinionStage::disappear:
        c->kill();
        break;
    }
    return true;
}

//void eGodMinionAction::lookForMonster() {
//    auto& board = eGodMinionAction::board();
//    const auto& ms = board.monsters(onCityId());
//    for(const auto m : ms) {
//        if(m->dead()) continue;
//        huntMonster(m, false);
//    }
//}

void eGodMinionAction::lookForSoldier() {
    auto& board = eGodMinionAction::board();
    const auto tid = character()->teamId();
    const auto cid = onCityId();
    const auto& chars = board.characters();
    for(const auto& cc : chars) {
        if(cc->dead()) continue;
        if(!cc->isSoldier()) continue;
        if(!eTeamIdHelpers::isEnemy(cc->teamId(), tid)) continue;
        if(cc->onCityId() != cid) continue;
        mStage = eGodMinionStage::huntSoldier;
        huntSoldier(cc);
        return;
    }
}

void eGodMinionAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eGodMinionAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    eGodMonsterAction::resumeFromSavedState();
}

void eGodMinionAction::serializeFields(eSaveArchive& ar) {
    eGodMonsterAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("lookForSoldier", mLookForSoldier);
    ar.field("approachCheck", mApproachCheck);
}

//void eGodMinionAction::lookForMonsterFight() {
//    const auto c = character();
//    const auto ct = c->tile();
//    if(!ct) return;
//    const int tx = ct->x();
//    const int ty = ct->y();
//    auto& board = c->getBoard();
//    const auto tid = c->teamId();
//    const int range = 5;
//    for(int i = -range; i <= range; i++) {
//        for(int j = -range; j <= range; j++) {
//            const auto t = board.tile(tx + i, ty + j);
//            if(!t) continue;
//            const auto& chars = t->characters();
//            for(const auto& cc : chars) {
//                if(cc->dead()) continue;
//                const auto cctype = cc->type();
//                bool monster;
//                eMonster::sCharacterToMonsterType(cctype, &monster);
//                if(!monster) continue;
//                const auto cctid = cc->teamId();
//                if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
//                const auto m = static_cast<eMonster*>(cc.get());
//                const bool r = fightMonster(m);
//                if(r) return;
//            }
//        }
//    }
//}
//
//bool eGodMinionAction::fightMonster(eMonster* const m) {
//    const auto c = character();
//    const vec2d cpos{c->absX(), c->absY()};
//    const vec2d mpos{m->absX(), m->absY()};
//    const vec2d posdif = mpos - cpos;
//    const double dist = posdif.length();
//    const double range = 5.;
//    if(dist > range) return false;
//    const auto angle = posdif.angle();
//    const auto o = sAngleOrientation(angle);
//    c->setOrientation(o);
//    c->setActionType(eCharacterActionType::fight);
//    stdsptr<eCharacterAction> ca;
//    const int fightTime = 5000;
//    const int attackTime = 1000;
//    const auto gm = e::make_shared<eGodMonsterActionInd>(c);
//    gm->spawnTimedMissiles(eCharacterActionType::fight,
//                           c->type(), attackTime, m,
//                           nullptr, nullptr, nullptr,
//                           fightTime);
//    ca = gm;
//    const auto mdie = std::make_shared<eHA_fightMonsterDie>(
//                          board(), m);
//    ca->setFailAction(mdie);
//    ca->setFinishAction(mdie);
//    setCurrentAction(ca);
//
//    mStage = eGodMinionStage::fightMonster;
//
//    return true;
//}
//
//void eGodMinionAction::huntMonster(eMonster* const m, const bool second) {
//    const auto mt = m->tile();
//    if(!mt) return;
//    const auto mtype = m->type();
//
//    const auto c = character();
//
//    const auto finish = std::make_shared<eGMinA_huntMonsterFinish>(
//                            board(), this);
//
//    const auto a = e::make_shared<eMoveToAction>(c);
//    a->setStateRelevance(eStateRelevance::buildings |
//                         eStateRelevance::terrain);
//    a->setFailAction(finish);
//    a->setFinishAction(finish);
//    const stdptr<eGodMinionAction> tptr(this);
//    const stdptr<eMonster> mptr(m);
//    a->setFoundAction([tptr, mptr, this, a, c, second]() {
//        if(!tptr || !mptr) return;
//        if(second) {
//            mStage = eGodMinionStage::huntMonster;
//            c->setActionType(eCharacterActionType::walk);
//        } else {
//            huntMonster(mptr, true);
//        }
//    });
//    a->setRemoveLastTurn(true);
//    a->setWait(false);
//    if(mtype == eCharacterType::scylla ||
//       mtype == eCharacterType::kraken) {
//        a->start(mt, eWalkableObject::sCreateWaterAndDefault());
//    } else {
//        a->start(mt, eWalkableObject::sCreateDefault());
//    }
//    if(second) {
//        setCurrentAction(a);
//    }
//}

void eGodMinionAction::lookForSoldierFight() {
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
                if(!cc->isSoldier()) continue;
                const auto cctid = cc->teamId();
                if(!eTeamIdHelpers::isEnemy(cctid, tid)) continue;
                const bool r = fightSoldier(cc.get());
                if(r) return;
            }
        }
    }
}

bool eGodMinionAction::fightSoldier(eCharacter* const s) {
    const auto c = character();
    const vec2d cpos{c->absX(), c->absY()};
    const vec2d spos{s->absX(), s->absY()};
    const vec2d posdif = spos - cpos;
    const double dist = posdif.length();

    // 75% chance to switch mode, 25% to repeat — prevents flip-flopping
    const bool goMelee = (eRand::rand() % 4 == 0) ? mLastFightMelee : !mLastFightMelee;
    const double maxDist = goMelee ? 1.5 : 5.;
    if(dist > maxDist) return false;

    const auto angle = posdif.angle();
    const auto o = sAngleOrientation(angle);
    c->setOrientation(o);
    c->setActionType(eCharacterActionType::fight);

    stdsptr<eCharacterAction> ca;
    const int fightTime = 3000 + (eRand::rand() % 7001); // 3–10s
    if(goMelee) {
        const auto w = e::make_shared<eWaitAction>(c);
        w->setTime(fightTime);
        ca = w;
        if(!s->fighting()) {
            const auto mo = !o;
            s->setOrientation(mo);
            s->setActionType(eCharacterActionType::fight);
            const auto sw = e::make_shared<eWaitAction>(s);
            s->setAction(sw);
        }
    } else {
        const int attackTime = 1000;
        const auto hitAct = std::make_shared<eLookForAttackGodAct>(board(), c);
        hitAct->find(s->tile());
        const auto gm = e::make_shared<eGodMonsterActionInd>(c);
        gm->spawnTimedMissiles(eCharacterActionType::fight2,
                               c->type(), attackTime, s,
                               nullptr, hitAct, nullptr,
                               fightTime);
        ca = gm;
    }

    const auto cb = std::make_shared<eGMinA_fightSoldierDie>(board(), this, s, goMelee);
    ca->setFailAction(cb);
    ca->setFinishAction(cb);
    setCurrentAction(ca);

    mLastFightMelee = goMelee;
    mStage = eGodMinionStage::fightSoldier;
    return true;
}

void eGodMinionAction::fightSoldierContinue(eCharacter* const s) {
    const bool r = fightSoldier(s);
    if(!r) {
        mStage = eGodMinionStage::huntSoldier;
        huntSoldier(s);
    }
}

void eGodMinionAction::huntSoldier(eCharacter* const s) {
    const auto st = s->tile();
    if(!st) return;

    const auto c = character();

    const auto finish = std::make_shared<eGMinA_huntSoldierFinish>(
                            board(), this);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    const stdptr<eGodMinionAction> tptr(this);
    const stdptr<eCharacter> sptr(s);
    a->setFoundAction([tptr, sptr, this, c]() {
        if(!tptr || !sptr) return;
        mStage = eGodMinionStage::huntSoldier;
        c->setActionType(eCharacterActionType::walk);
    });
    a->setRemoveLastTurn(true);
    a->setWait(false);
    a->start(st, eWalkableObject::sCreateDefault());
    setCurrentAction(a);
}
