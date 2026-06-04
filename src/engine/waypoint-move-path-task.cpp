#include "waypoint-move-path-task.h"

#include "thread/ethreadboard.h"
#include "engine/game-board.h"

#include "buildings/epatrolbuildingbase.h"

#include "engine/epathfinder.h"

eWaypointMovePathTask::eWaypointMovePathTask(ePatrolBuildingBase * const b,
                                         const eAction &finish) :
    eTask(b->cityId()),
    mB(b), mFinish(finish) {
    setRelevance(eStateRelevance::buildings);
    const auto walkable = WalkableObject::sCreateRoadAvenue();
    mWalkable = WalkableObject::sCreateRect(b, walkable);

    mBothDirections = b->bothDirections();
    {
        const auto cid = b->cityId();
        auto& board = b->getBoard();
        const auto c = board.boardCityWithId(cid);
        mTileBRect = c->tileBRect();
    }
    mWaypoints = mB->patrolWaypoints();
    {
        const auto c = b->patrolStartTile();
        const int x = c->x();
        const int y = c->y();
        mStartWaypoint = {x, y};
    }
}

void eWaypointMovePathTask::run(eThreadBoard &data) {
    const int maxDistance = eNumbers::sPatrolerMaxDistance;

    for(int j = 0; j < (mBothDirections ? 2 : 1); j++) {
        int distance = 0;
        ePatrolWaypoint last = mStartWaypoint;
        const int iMax = mWaypoints.size();
        for(int i = 0; i < iMax; i++) {
            const auto& from = j == 0 ? (i == 0 ? mStartWaypoint : mWaypoints[i - 1]) :
                                        (i == 0 ? mStartWaypoint : mWaypoints[iMax - i]);
            const auto& to = j == 0 ? (i == iMax ? mStartWaypoint : mWaypoints[i]) :
                                      (i == iMax ? mStartWaypoint : mWaypoints[iMax - i - 1]);
            const bool r = runImpl(data, j == 0 ? mPath : mReversePath,
                                   from, to, distance, maxDistance, last);
            if(!r || i == iMax - 1) {
                runImpl(data, j == 0 ? mPath : mReversePath,
                        last, mStartWaypoint, distance, 1000, last);
                break;
            }
        }
    }
}

void eWaypointMovePathTask::finish() {
    if(!mB) return;
    mB->setPath(mPath, mReversePath);
    if(mFinish) mFinish();
}

bool eWaypointMovePathTask::runImpl(eThreadBoard& data,
                                  ePath &path,
                                  const ePatrolWaypoint& from,
                                  const ePatrolWaypoint& to,
                                  int& distance,
                                  const int maxDistance,
                                  ePatrolWaypoint& last) {
    last = from;
    const auto startT = data.tile(from.fX, from.fY);

    ePathFinder pf0([&](eTileBase* const t) {
        return mWalkable->walkable(t);
    }, [&](eTileBase* const t) {
        return t->x() == to.fX && t->y() == to.fY;
    });

    const bool onlyDiagonal = true;
    const bool r = pf0.findPath(mTileBRect,
                                startT, 2*maxDistance,
                                onlyDiagonal, data.width(), data.height(),
                                eWalkableHelpers::sRoadAvenueTileDistance);
    if(r) {
        ePath tpath;
        const bool r = pf0.extractPath(tpath);
        if(!r) return false;
        path.reserve(path.size() + tpath.size());
        const int iMin = tpath.size() - 1;
        for(int i = iMin; i >= 0; i--) {
            const auto o = tpath[i];
            path.insert(path.begin(), o);
            const auto tile = data.tile(last.fX, last.fY);
            const bool road = tile->hasRoad();
            last.moveInDirection(o);
            if(road) distance++;
            if(distance >= maxDistance) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}
