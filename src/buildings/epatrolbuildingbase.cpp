#include "epatrolbuildingbase.h"

#include "engine/game-board.h"
#include "engine/etile.h"
#include "engine/waypoint-move-path-task.h"
#include "fileIO/esavearchive.h"

eTile* ePatrolBuildingBase::patrolStartTile() const {
    return centerTile();
}

ePatrolBuildingBase::ePatrolBuildingBase(
        GameBoard& board,
        const eCharGenerator& charGen,
        const eActGenerator& actGen,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    eEmployingBuilding(board, type, sw, sh, maxEmployees, cid),
    mCharGenerator(charGen),
    mActGenerator(actGen) {

}

stdsptr<eCharacterAction>
ePatrolBuildingBase::sDefaultActGenerator(
           eCharacter* const c,
           ePatrolBuildingBase* const b,
           const std::vector<eOrientation>& path,
           const stdsptr<eDirectionTimes>& dirTimes) {
    return e::make_shared<ePatrolAction>(c, b, path, dirTimes);
}

ePatrolBuildingBase::ePatrolBuildingBase(GameBoard& board,
        const eCharGenerator& charGen,
        const eBuildingType type,
        const int sw, const int sh,
        const int maxEmployees,
        const eCityId cid) :
    ePatrolBuildingBase(board, charGen, sDefaultActGenerator,
                        type, sw, sh, maxEmployees, cid) {}

ePatrolBuildingBase::~ePatrolBuildingBase() {
    if(mChar) mChar->kill();
}

void ePatrolBuildingBase::timeChanged(const int by) {
    if(mSpawnPatrolers && enabled()) {
        if(!mChar) {
            mSpawnTimer += by*effectiveness();
            if(mSpawnTimer > spawnCooldown()) {
                mSpawnTimer = 0;
                const bool r = updatePathIfNeeded();
                if(r) mSpawnOnPathSet = true;
                else spawn();
            }
        }
    }
    eEmployingBuilding::timeChanged(by);
}

using ePatrolWaypoints = std::vector<ePatrolWaypoint>;
ePatrolWaypoints ePatrolBuildingBase::reversePatrolWaypoints() const {
    auto waypoints = mPatrolWaypoints;
    std::reverse(waypoints.begin(), waypoints.end());
    return waypoints;
}

void ePatrolBuildingBase::setPatrolWaypoints(const ePatrolWaypoints &waypoints) {
    mPatrolWaypoints = waypoints;
}

void ePatrolBuildingBase::setBothDirections(const bool both) {
    mBothDirections = both;
}

bool ePatrolBuildingBase::spawn() {
    if(!mCharGenerator || !mActGenerator) return false;
    const auto chr = mCharGenerator();
    mChar = chr.get();
    if(!mChar) return false;
    chr->setBothCityIds(cityId());
    if(mPatrolWaypoints.empty()) {
        eTile* t = nullptr;
        const auto bt = type();
        if(bt == eBuildingType::commonAgora ||
           bt == eBuildingType::grandAgora) {
            t = patrolStartTile();
        } else {
            const auto ts = surroundingRoad(false, true);
            if(!ts.empty()) {
                const int tss = ts.size();
                if(++mSpawnRoadId >= tss) mSpawnRoadId = 0;
                t = ts[mSpawnRoadId];
            }
        }
        if(!t) return false;
        chr->changeTile(t);
    } else {
        chr->changeTile(patrolStartTile());
    }
    bool reverse = false;
    if(mBothDirections) {
        mLastDirection = !mLastDirection;
        if(mLastDirection) reverse = true;
    }
    const auto a = mActGenerator(chr.get(), this,
                                 reverse ? mReversePath : mPath,
                                 mDirTimes);
    chr->setAction(a);
    return true;
}

void ePatrolBuildingBase::setSpawnPatrolers(const bool s) {
    mSpawnPatrolers = s;
}

void ePatrolBuildingBase::resetSpawnTimer() {
    mSpawnTimer = 0;
}

bool ePatrolBuildingBase::spawnsPatrolers() const {
    return mCharGenerator && mActGenerator;
}

void ePatrolBuildingBase::serializeFields(eSaveArchive& ar) {
    eEmployingBuilding::serializeFields(ar);
    ar.field("bothDirections", mBothDirections);
    ar.field("lastDirection", mLastDirection);
    ar.field("spawnPatrolers", mSpawnPatrolers);
    ar.field("spawnTime", mSpawnTimer);
    ar.field("spawnRoadId", mSpawnRoadId);

    ar.directionTimesField("directionTimes", getBoard(), mDirTimes);

    ar.characterField("patroler", &getBoard(), mChar);

    ar.arrayField("patrolGuides", mPatrolWaypoints,
        [](eSaveArchive& itemAr, ePatrolWaypoint& waypoint) {
            itemAr.field("x", waypoint.fX);
            itemAr.field("y", waypoint.fY);
        });
}

bool ePatrolBuildingBase::updatePathIfNeeded() {
    if(mPatrolWaypoints.empty()) {
        mPath.clear();
        mReversePath.clear();
        return false;
    }
    auto& board = getBoard();
    const auto cid = cityId();
    const auto c = board.boardCityWithId(cid);
    const int s = c->roadState();
    if(s == mPathState) return false;
    mPathState = s;
    return updatePath();
}

bool ePatrolBuildingBase::updatePath(const eAction& finish) {
    if(mPatrolWaypoints.empty()) {
        mPath.clear();
        mReversePath.clear();
        return false;
    }
    auto& board = getBoard();
    auto& tp = board.threadPool();
    const auto task = new eWaypointMovePathTask(this, finish);
    tp.queueTask(task);
    return true;
}

void ePatrolBuildingBase::setPath(const std::vector<eOrientation> &path,
                                  const std::vector<eOrientation> &reversePath) {
    mPath = path;
    mReversePath = reversePath;
    if(mSpawnOnPathSet) {
        mSpawnOnPathSet = false;
        spawn();
    }
}
