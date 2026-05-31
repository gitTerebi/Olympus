#ifndef EGUIDEDMOVEPATHTASK_H
#define EGUIDEDMOVEPATHTASK_H

#include <vector>

#include "epatrolguide.h"

#include "engine/etask.h"
#include "engine/thread/ethreadtile.h"

class ePatrolBuildingBase;
class ePathFinderBase;

class eGuidedMovePathTask : public eTask {
public:
    using ePath = std::vector<eOrientation>;
    using eTileGetter = std::function<eThreadTile*(eThreadBoard&)>;
    using eAction = std::function<void()>;
    eGuidedMovePathTask(ePatrolBuildingBase* const b,
                        const eAction& finish = nullptr);
protected:
    void run(eThreadBoard& data) override;
    void finish() override;
private:
    bool runImpl(eThreadBoard& data,
                 ePath &path,
                 const ePatrolGuide& from,
                 const ePatrolGuide& to,
                 int& distance,
                 const int maxDistance,
                 ePatrolGuide& last);

    const stdptr<ePatrolBuildingBase> mB;
    const eAction mFinish;

    stdsptr<eWalkableObject> mWalkable;
    bool mBothDirections;
    ePatrolGuide mStartGuide;
    std::vector<ePatrolGuide> mGuides;
    SDL_Rect mTileBRect;

    ePath mPath;
    ePath mReversePath;
};

#endif // EGUIDEDMOVEPATHTASK_H
