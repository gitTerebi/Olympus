#include "god-visit-action.h"
#include "fileIO/save-archive.h"

#include "characters/echaracter.h"
#include "numbers.h"

GodVisitAction::GodVisitAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::godVisitAction) {}

void GodVisitAction::increment(const int by) {
    const int attackPeriod = Numbers::sGodVisitSoldierAttackPeriod;
    const int attackRange = Numbers::sGodVisitSoldierAttackRange;
    const int blessPeriod = Numbers::sGodVisitBlessPeriod;
    const int blessRange = Numbers::sGodVisitBlessRange;

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
        if(len >= 5) patrol(nullptr, Numbers::sGodVisitPatrolDistance);
        else moveAround(nullptr, Numbers::sGodVisitMoveAroundTime);
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

void GodVisitAction::serializeFields(SaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("lookForBless", mLookForBless);
    ar.field("lookForSoldierAttack", mLookForSoldierAttack);
}
