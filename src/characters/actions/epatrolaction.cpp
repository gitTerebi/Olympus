#include "epatrolaction.h"

#include "../echaracter.h"
#include "engine/etile.h"
#include "epatrolmoveaction.h"
#include "buildings/epatrolbuildingbase.h"
#include "fileIO/save-archive.h"

ePatrolAction::ePatrolAction(eCharacter* const c,
                             ePatrolBuildingBase* const b,
                             const std::vector<eOrientation>& path,
                             const stdsptr<eDirectionTimes>& dirTimes,
                             const eCharActionType at) :
    eActionWithComeback(c, b ? b->centerTile() : nullptr, at),
    mPath(path), mDirTimes(dirTimes), mBuilding(b) {
    setFinishOnComeback(true);
    setDiagonalOnly(true);
}

ePatrolAction::ePatrolAction(eCharacter* const c,
                             const eCharActionType at) :
    ePatrolAction(c, nullptr, {}, nullptr, at) {}

bool ePatrolAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;
    if(mDone) {
        goBackDecision();
    } else {
        patrol();
    }
    return true;
}

void ePatrolAction::serializeFields(SaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.arrayField("path", mPath, [](SaveArchive& itemAr, eOrientation& o) {
        itemAr.field("orientation", o);
    });
    ar.buildingAsField("building", &board(), mBuilding);
    ar.field("done", mDone);
    ar.directionTimesField("dirTimes", board(), mDirTimes);
    ar.field("patrolStage", mStage);
}

void ePatrolAction::resumeFromSavedState() {
    switch(mStage) {
    case ePatrolActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        break;
    case ePatrolActionStage::patrolling:
        patrol();
        break;
    case ePatrolActionStage::goingBack:
        goBackDecision();
        break;
    }
}

void ePatrolAction::patrol() {
    const auto c = character();
    mStage = ePatrolActionStage::patrolling;
    c->setActionType(eCharacterActionType::walk);
    const auto failFunc = std::make_shared<ePA_patrolFail>(
                              board(), this);
    const auto finishFunc = std::make_shared<ePA_patrolFinish>(
                                board(), this);
    const int dist = mBuilding->maxDistance();
    if(mPath.empty()) {
        const auto bt = mBuilding->type();
        const bool isAgora = bt == eBuildingType::commonAgora ||
                             bt == eBuildingType::grandAgora;
        const auto walkable = isAgora ? WalkableObject::sCreateRoadblockNoAgora()
                                      : WalkableObject::sCreateRoadblock();
        const auto a = e::make_shared<ePatrolMoveAction>(
            c, true, walkable, mDirTimes);
        a->setFailAction(failFunc);
        a->setFinishAction(finishFunc);
        a->setMaxWalkDistance(dist);
        setCurrentAction(a);
    } else {
        auto walkable = WalkableObject::sCreateRoadAvenue();
        walkable = WalkableObject::sCreateRect(mBuilding, walkable);
        const auto a = e::make_shared<eMovePathAction>(
            c, mPath, walkable);
        a->setFailAction(failFunc);
        a->setFinishAction(finishFunc);
        setCurrentAction(a);
    }
}

void ePatrolAction::goBackDecision(const stdsptr<WalkableObject>& w) {
    mStage = ePatrolActionStage::goingBack;
    goBack(mBuilding, w);
}
