#include "path-find-task.h"

#include "engine/epathfinder.h"
#include "engine/eknownendpathfinder.h"
#include "characters/actions/walkable/walkable-object.h"
#include "walkable-helpers.h"

using eTileDistance = std::function<int(eTileBase* const)>;
eTileDistance tileDist(const stdsptr<WalkableObject>& w,
                       const eTileDistance& d) {
    if(!d && w && w->rootType() == eWalkableObjectType::roadAvenue) {
        return WalkableHelpers::sRoadAvenueTileDistance;
    } else {
        return d;
    }
}

PathFindTask::PathFindTask(const eCityId cid,
                             const SDL_Rect& tileBRect,
                             const eTileGetter& startTile,
                             const stdsptr<WalkableObject>& tileWalkable,
                             const eTileChecker& endTileFunc,
                             const eFinishFunc& finishFunc,
                             const eFailFunc& failFunc,
                             const bool onlyDiagonal,
                             const int range,
                             const eTileDistance& distance,
                             const eTileGetter& endTile,
                             const bool findAll) :
    eTask(cid),
    mTileBRect(tileBRect),
    mStartTile(startTile),
    mEndTile(endTile),
    mTileWalkable(tileWalkable),
    mEndTileFunc(endTileFunc),
    mFinish(finishFunc),
    mFailFunc(failFunc),
    mOnlyDiagonal(onlyDiagonal),
    mRange(range),
    mDistance(tileDist(tileWalkable, distance)),
    mFindAll(findAll) {}

void PathFindTask::run(eThreadBoard& data) {
    if(mEndTile) {
        const auto endT = mEndTile(data);
        eKnownEndPathFinder pf0(
        [&](eTileBase* const t) {
            return mTileWalkable->walkable(t);
        }, endT);
        runImpl(data, pf0);
    } else if(mEndTileFunc) {
        ePathFinder pf0(
        [&](eTileBase* const t) {
            return mTileWalkable->walkable(t);
        },
        [&](eTileBase* const t) {
           return mEndTileFunc(static_cast<eThreadTile*>(t));
        });
        runImpl(data, pf0);
    }
}

void PathFindTask::finish() {
    if(mFoundFunc) mFoundFunc(mFound);
    if(mR) mFinish(mPath);
    else mFailFunc();
}

void PathFindTask::runImpl(eThreadBoard& data, ePathFinderBase& pf0) {
    if(mFindAll) pf0.setMode(ePathFinderMode::findAll);
    const auto startT = mStartTile(data);
    const bool r = pf0.findPath(mTileBRect,
                                startT, mRange, mOnlyDiagonal,
                                data.width(), data.height(),
                                mDistance);
    if(r) {
        mR = pf0.extractPath(mPath);
    } else {
        mR = false;
    }
    if(mFindAll && mFoundFunc) {
        ePathFindData fd;
        pf0.extractData(fd);
        mFound.reserve(fd.fFoundAll.size());
        for(const auto& f : fd.fFoundAll) {
            const auto t = f.first;
            mFound.push_back({{t->x(), t->y()}, f.second});
        }
    }
}
