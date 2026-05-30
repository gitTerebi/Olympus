#include "god-minion-action.h"

#include "fileIO/esavearchive.h"

#include "characters/monsters/emonster.h"
#include "characters/actions/emovetoaction.h"
#include "characters/actions/eheroaction.h"
#include "characters/gods/actions/god-action.h"
#include "engine/game-board.h"
#include "vec2.h"

eGodMinionAction::eGodMinionAction(eCharacter* const c) :
    eGodMonsterAction(c, eCharActionType::godMinionAction) {}

void eGodMinionAction::increment(const int by) {
    if(mStage != eGodMinionStage::huntMonster &&
       mStage != eGodMinionStage::fightMonster) {
        const int lookForMonsterCheck = 5000;
        mLookForMonster += by;
        if(mLookForMonster > lookForMonsterCheck) {
            mLookForMonster -= lookForMonsterCheck;
            lookForMonster();
        }
    }
    if(mStage == eGodMinionStage::huntMonster) {
        lookForMonsterFight();
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

void eGodMinionAction::lookForMonster() {
    auto& board = eGodMinionAction::board();
    const auto& ms = board.monsters(onCityId());
    for(const auto m : ms) {
        if(m->dead()) continue;
        huntMonster(m, false);
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
    ar.field("lookForMonster", mLookForMonster);
}

void eGodMinionAction::lookForMonsterFight() {
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

bool eGodMinionAction::fightMonster(eMonster* const m) {
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
    const int attackTime = 1000;
    const auto gm = e::make_shared<eGodMonsterActionInd>(c);
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

    mStage = eGodMinionStage::fightMonster;

    return true;
}

void eGodMinionAction::huntMonster(eMonster* const m, const bool second) {
    const auto mt = m->tile();
    if(!mt) return;
    const auto mtype = m->type();

    const auto c = character();

    const auto finish = std::make_shared<eGMinA_huntMonsterFinish>(
                            board(), this);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    const stdptr<eGodMinionAction> tptr(this);
    const stdptr<eMonster> mptr(m);
    a->setFoundAction([tptr, mptr, this, a, c, second]() {
        if(!tptr || !mptr) return;
        if(second) {
            mStage = eGodMinionStage::huntMonster;
            c->setActionType(eCharacterActionType::walk);
        } else {
            huntMonster(mptr, true);
        }
    });
    a->setRemoveLastTurn(true);
    a->setWait(false);
    if(mtype == eCharacterType::scylla ||
       mtype == eCharacterType::kraken) {
        a->start(mt, eWalkableObject::sCreateWaterAndDefault());
    } else {
        a->start(mt, eWalkableObject::sCreateDefault());
    }
    if(second) {
        setCurrentAction(a);
    }
}
