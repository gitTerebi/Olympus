#include "god-visit-action.h"
#include "fileIO/esavearchive.h"

#include "characters/echaracter.h"
#include "enumbers.h"

GodVisitAction::GodVisitAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::godVisitAction) {}

void GodVisitAction::increment(const int by) {
    const int attackPeriod = eNumbers::sGodVisitSoldierAttackPeriod;
    const int attackRange = eNumbers::sGodVisitSoldierAttackRange;
    const int blessPeriod = eNumbers::sGodVisitBlessPeriod;
    const int blessRange = eNumbers::sGodVisitBlessRange;

    bool r = lookForTargetedBlessCurse(by, mLookForBless, blessPeriod, blessRange, 1);
    if(!r) lookForSoldierAttack(by, mLookForSoldierAttack, attackPeriod, attackRange);

    eGodAction::increment(by);
}

bool GodVisitAction::decide() {
    const auto c = character();
    switch(mStage) {
    case GodVisitStage::none:
        mStage = GodVisitStage::appear;
        randomPlaceOnBoard();
        if(!c->tile()) {
            c->kill();
        } else {
            appear();
        }
        break;
    case GodVisitStage::appear: {
        mStage = GodVisitStage::patrol;
        const auto tile = c->tile();
        const int len = tile->roadLength(5);
        if(len >= 5) patrol(nullptr, eNumbers::sGodVisitPatrolDistance);
        else moveAround(nullptr, eNumbers::sGodVisitMoveAroundTime);
    }   break;
    case GodVisitStage::patrol:
        mStage = GodVisitStage::disappear;
        disappear();
        break;
    case GodVisitStage::disappear:
        c->kill();
        break;
    }
    return true;
}

void GodVisitAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void GodVisitAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case GodVisitStage::none:
    case GodVisitStage::appear:
    case GodVisitStage::patrol:
    case GodVisitStage::disappear:
        eGodAction::resumeFromSavedState();
        return;
    }
}

void GodVisitAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("lookForBless", mLookForBless);
    ar.field("lookForSoldierAttack", mLookForSoldierAttack);
}
