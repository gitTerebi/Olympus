#include "eaiconnectwithroadtask.h"

#include "engine/egameboard.h"
#include "engine/epathfinder.h"

#include "buildings/eroad.h"

eAIConnectWithRoadTask::eAIConnectWithRoadTask(
        eGameBoard& board,
        const ePoints& pts,
        const ePlayerId pid,
        const eCityId cid) :
    eTask(cid), mBoard(board), mPoints(pts), mPid(pid) {
}

void eAIConnectWithRoadTask::run(eThreadBoard& data) {
    const auto& pts = mPoints.back();
    const int r1x = pts.first.x;
    const int r1y = pts.first.y;
    const int r2x = pts.second.x;
    const int r2y = pts.second.y;
    ePathFinder p([](eTileBase* const t) {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildableAfterClear & terr);
        if(!tr) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::road ||
                       bt == eBuildingType::none;
        if(!r) return false;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        return true;
    }, [&](eTileBase* const t) {
        return t->x() == r2x && t->y() == r2y;
    });

    const auto startTile = data.tile(r1x, r1y);
    const int w = data.width();
    const int h = data.height();
    const auto distance = [](eTileBase* const tile) {
        if(tile->hasRoad()) return 1;
        return 6;
    };
    const bool r = p.findPath({0, 0, w, h}, startTile, 10000, true, w, h, distance);
    if(!r) return;
    p.extractPath(mPath);
}
#include "buildings/epark.h"
void eAIConnectWithRoadTask::finish() {
    const auto& pts = mPoints.back();
    const int r1x = pts.first.x;
    const int r1y = pts.first.y;
    const auto startTile = mBoard.tile(r1x, r1y);
    eTile* t = startTile;
    const auto buildRoad = [&]() {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildable & terr);
        if(!tr) t->setTerrain(eTerrain::dry);
        const auto bt = t->underBuildingType();
        if(bt != eBuildingType::none &&
           bt != eBuildingType::road) {
            t->underBuilding()->erase();
            mBoard.build(t->x(), t->y(), 1, 1, cid(), mPid,
                              [&]() { return e::make_shared<ePark>(mBoard, cid()); },
                              false, true);
        } else {
            mBoard.build(t->x(), t->y(), 1, 1, cid(), mPid,
                  [&]() { return e::make_shared<eRoad>(mBoard, cid()); },
                  false, true);
        }
    };
    for(int i = mPath.size() - 1; i >= 0; i--) {
        if(!t) break;
        buildRoad();
        t = t->neighbour<eTile>(mPath[i]);
    }
    if(t) buildRoad();

    mPoints.pop_back();
    if(mPoints.empty()) return;
    auto& tp = mBoard.threadPool();
    tp.scheduleDataUpdate();
    const auto task = new eAIConnectWithRoadTask(
                          mBoard, mPoints, mPid, cid());
    tp.queueTask(task);
}
