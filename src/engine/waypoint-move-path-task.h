#ifndef WAYPOINT_MOVE_PATH_TASK_H
#define WAYPOINT_MOVE_PATH_TASK_H

#include <vector>

#include "patrol-waypoint.h"

#include "engine/etask.h"
#include "engine/thread/ethreadtile.h"

class ePatrolBuildingBase;
class ePathFinderBase;

class eWaypointMovePathTask : public eTask {
public:
    using ePath = std::vector<eOrientation>;
    using eTileGetter = std::function<eThreadTile*(eThreadBoard&)>;
    using eAction = std::function<void()>;
    eWaypointMovePathTask(ePatrolBuildingBase* const b,
                        const eAction& finish = nullptr);
protected:
    void run(eThreadBoard& data) override;
    void finish() override;
private:
    bool runImpl(eThreadBoard& data,
                 ePath &path,
                 const ePatrolWaypoint& from,
                 const ePatrolWaypoint& to,
                 int& distance,
                 const int maxDistance,
                 ePatrolWaypoint& last);

    const stdptr<ePatrolBuildingBase> mB;
    const eAction mFinish;

    stdsptr<WalkableObject> mWalkable;
    bool mBothDirections;
    ePatrolWaypoint mStartWaypoint;
    std::vector<ePatrolWaypoint> mWaypoints;
    SDL_Rect mTileBRect;

    ePath mPath;
    ePath mReversePath;
};

#endif // WAYPOINT_MOVE_PATH_TASK_H
