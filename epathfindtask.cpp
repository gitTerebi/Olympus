#include "epathfindtask.h"

#include "engine/epathfinder.h"
#include "engine/eknownendpathfinder.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "ewalkablehelpers.h"

using eTileDistance = std::function<int(eTileBase* const)>;
eTileDistance tileDist(const stdsptr<eWalkableObject>& w,
                       const eTileDistance& d) {
    if(!d && w && w->rootType() == eWalkableObjectType::roadAvenue) {
        return eWalkableHelpers::sRoadAvenueTileDistance;
    } else {
        return d;
    }
}

ePathFindTask::ePathFindTask(const eCityId cid,
                             const SDL_Rect& tileBRect,
                             const eTileGetter& startTile,
                             const stdsptr<eWalkableObject>& tileWalkable,
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

void ePathFindTask::run(eThreadBoard& data) {
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

void ePathFindTask::finish() {
    if(mR) mFinish(mPath);
    else mFailFunc();
}

void ePathFindTask::runImpl(eThreadBoard& data, ePathFinderBase& pf0) {
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
}
