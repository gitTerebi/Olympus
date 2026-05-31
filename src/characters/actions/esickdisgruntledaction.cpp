#include "esickdisgruntledaction.h"

#include "../echaracter.h"
#include "buildings/small-house.h"
#include "epatrolmoveaction.h"
#include "move-around-action.h"
#include "fileIO/esavearchive.h"

eSickDisgruntledAction::eSickDisgruntledAction(eCharacter* const c,
                                               SmallHouse* const ch) :
    eActionWithComeback(c, ch ? ch->centerTile() : nullptr,
                        eCharActionType::sickDisgruntledAction),
    mBuilding(ch) {
    setFinishOnComeback(true);
}

bool eSickDisgruntledAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;
    if(mGoBackNext) {
        goBackDecision();
    } else {
        patrol();
        mGoBackNext = true;
    }
    return true;
}

void eSickDisgruntledAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.buildingAsField("building", &board(), mBuilding);
    ar.field("goBackNext", mGoBackNext);
    ar.field("sickDisgruntledStage", mStage);
}

void eSickDisgruntledAction::resumeFromSavedState() {
    switch(mStage) {
    case eSickDisgruntledActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        break;
    case eSickDisgruntledActionStage::patrolling:
        patrol();
        break;
    case eSickDisgruntledActionStage::goingBack:
        goBackDecision();
        break;
    }
}

void eSickDisgruntledAction::patrol() {
    const auto c = character();
    mStage = eSickDisgruntledActionStage::patrolling;
    if(!c) return;
    const auto t = c->tile();
    if(!t) return;
    c->setActionType(eCharacterActionType::walk);
    if(t->hasRoad()) {
        const auto a = e::make_shared<ePatrolMoveAction>(
                           c, true,
                           eWalkableObject::sCreateRoad());
        a->setMaxWalkDistance(50);
        setCurrentAction(a);
    } else {
        auto walkable = eWalkableObject::sCreateDefault();
        if(const auto ub = t->underBuilding()) {
            walkable = eWalkableObject::sCreateRect(ub, walkable);
        }

        const auto a = e::make_shared<MoveAroundAction>(
                           c, t->x(), t->y(),
                           eWalkableObject::sCreateDefault());
        a->setTime(5000);
        a->setMaxDistance(8);
        setCurrentAction(a);
    }
}

void eSickDisgruntledAction::goBackDecision(const stdsptr<eWalkableObject>& w) {
    mStage = eSickDisgruntledActionStage::goingBack;
    goBack(mBuilding, w);
}
