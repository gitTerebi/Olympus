#include "epatrolaction.h"

#include <string>

#include "../echaracter.h"
#include "engine/etile.h"
#include "epatrolmoveaction.h"
#include "buildings/epatrolbuildingbase.h"
#include "fileIO/esavearchive.h"

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
    if(!mBuilding) {
        setState(eCharacterActionState::finished);
        return true;
    }
    const bool r = eActionWithComeback::decide();
    if(r) return r;
    if(mDone) {
        goBackDecision();
    } else {
        patrol();
    }
    return true;
}

void ePatrolAction::read(eReadStream& src) {
    eActionWithComeback::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void ePatrolAction::write(eWriteStream& dst) const {
    eActionWithComeback::write(dst);
    eSaveArchive ar(dst);
    const_cast<ePatrolAction*>(this)->serialize(ar);
}

void ePatrolAction::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        int n;
        ar.field("n", n);
        mPath.clear();
        for(int i = 0; i < n; i++) {
            auto& o = mPath.emplace_back();
            ar.field("o", o);
        }
    } else {
        int n = static_cast<int>(mPath.size());
        ar.field("n", n);
        for(auto o : mPath) {
            ar.field("o", o);
        }
    }
    if(ar.reading()) {
        ar.readStream().readBuilding(&board(), [this](eBuilding* const b) {
            mBuilding = dynamic_cast<ePatrolBuildingBase*>(b);
        });
    } else {
        ar.writeStream().writeBuilding(mBuilding);
    }
    ar.field("mDone", mDone);
    if(ar.reading()) {
        mDirTimes = ar.readStream().readDirectionTimes(board());
    } else {
        ar.writeStream().writeDirectionTimes(mDirTimes.get());
    }
}

void ePatrolAction::serializeJson(eJsonArchive& ar) {
    eActionWithComeback::serializeJson(ar);
    int n = ar.writing() ? static_cast<int>(mPath.size()) : 0;
    ar.field("path.count", n);
    if(ar.reading()) mPath.resize(n);
    for(int i = 0; i < n; i++) {
        ar.field(("path." + std::to_string(i)).c_str(), mPath[i]);
    }
    eBuilding* b = mBuilding.get();
    ar.buildingRef("mBuilding", b, board());
    if(ar.reading()) mBuilding = dynamic_cast<ePatrolBuildingBase*>(b);
    ar.field("mDone", mDone);
    ar.directionTimesRef("mDirTimes", mDirTimes, board());
}

void ePatrolAction::patrol() {
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    const auto failFunc = std::make_shared<ePA_patrolFail>(
                              board(), this);
    const auto finishFunc = std::make_shared<ePA_patrolFinish>(
                                board(), this);
    const int dist = mBuilding->maxDistance();
    if(mPath.empty()) {
        const auto walkable = eWalkableObject::sCreateRoadblock();
        const auto a = e::make_shared<ePatrolMoveAction>(
            c, true, walkable, mDirTimes);
        a->setFailAction(failFunc);
        a->setFinishAction(finishFunc);
        a->setMaxWalkDistance(dist);
        setCurrentAction(a);
    } else {
        auto walkable = eWalkableObject::sCreateRoadAvenue();
        walkable = eWalkableObject::sCreateRect(mBuilding, walkable);
        const auto a = e::make_shared<eMovePathAction>(
            c, mPath, walkable);
        a->setFailAction(failFunc);
        a->setFinishAction(finishFunc);
        setCurrentAction(a);
    }
}

void ePatrolAction::goBackDecision(const stdsptr<eWalkableObject>& w) {
    goBack(mBuilding, w);
}
