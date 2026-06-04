#include "widgets/game-widget.h"


#include "engine/game-board.h"
#include "engine/etile.h"
#include "engine/epathfinder.h"
#include "buildings/epatrolbuildingbase.h"
#include "buildings/epatrolsourcebuilding.h"
#include "buildings/epatroltarget.h"
#include "characters/actions/walkable/walkable-object.h"
#include "enumbers.h"

using ePatrolWaypoints = std::vector<ePatrolWaypoint>;

void GameWidget::updateWaypointPath()
{
    if (!mPatrolBuilding)
    {
        mWaypointOutPath.clear();
        mWaypointReturnPath.clear();
        mWaypointOutPath1.clear();
        mWaypointReturnPath1.clear();
        return;
    }
    const bool r = mPatrolBuilding->updatePath([this]()
                                               {
        mWaypointOutPath.clear();
        mWaypointReturnPath.clear();
        mWaypointOutPath1.clear();
        mWaypointReturnPath1.clear();
        if(!mPatrolBuilding) return;
        const auto startTile = mPatrolBuilding->patrolStartTile();
        {
            const auto& path = mPatrolBuilding->path();
            auto lastTile = startTile;
            mWaypointOutPath.push_back(lastTile);
            const int iMin = path.size() - 1;
            for(int i = iMin; i >= 0; i--) {
                const auto o = path[i];
                lastTile = lastTile->neighbour<eTile>(o);
                mWaypointOutPath.push_back(lastTile);
            }
        }
        {
            const auto& path = mPatrolBuilding->reversePath();
            auto lastTile = startTile;
            mWaypointOutPath1.push_back(lastTile);
            const int iMin = path.size() - 1;
            for(int i = iMin; i >= 0; i--) {
                const auto o = path[i];
                lastTile = lastTile->neighbour<eTile>(o);
                mWaypointOutPath1.push_back(lastTile);
            }
        }

        const auto handleWaypointPath = [&](std::vector<eTile*>& excessPath,
                                          const ePatrolWaypoints& waypoints) {
            const auto startTile = mPatrolBuilding->patrolStartTile();
            auto lastTile = startTile;
            const auto handlePath = [&](eTile* const from, eTile* const to) {
                if(!from || !to) return false;
                const auto valid = [&](eTileBase* const t) {
                    const auto type = t->underBuildingType();
                    const bool hr = type == eBuildingType::road;
                    if(hr) return true;
                    const bool a = type == eBuildingType::avenue;
                    if(a) return true;
                    const auto tt = static_cast<eTile*>(t);
                    return tt->underBuilding() == mPatrolBuilding;
                };
                const auto final = [&](eTileBase* const t) {
                    return t->x() == from->x() && t->y() == from->y();
                };
                ePathFinder p(valid, final);
                const int w = mBoard->width();
                const int h = mBoard->height();
                const bool r = p.findPath({0, 0, w, h}, to, 100, true, w, h,
                                          eWalkableHelpers::sRoadAvenueTileDistance);
                if(!r) return false;
                std::vector<eTile*> path;
                p.extractPath(path, *mBoard);
                for(const auto p : path) {
                    excessPath.emplace_back(p);
                }
                return true;
            };
            for(const auto& waypoint : waypoints) {
                if(!lastTile) break;
                const auto waypointTile = mBoard->tile(waypoint.fX, waypoint.fY);
                if(!waypointTile) break;
                const bool r = handlePath(lastTile, waypointTile);
                if(!r) break;
                lastTile = waypointTile;
            }
        };
        {
            const auto& waypoints = mPatrolBuilding->patrolWaypoints();
            handleWaypointPath(mWaypointReturnPath, waypoints);
        }
        if(mPatrolBuilding->bothDirections()) {
            const auto waypoints = mPatrolBuilding->reversePatrolWaypoints();
            handleWaypointPath(mWaypointReturnPath1, waypoints);
        } });
    if (!r)
    {
        mWaypointOutPath.clear();
        mWaypointReturnPath.clear();
        mWaypointOutPath1.clear();
        mWaypointReturnPath1.clear();
    }
}

void GameWidget::updateDestinationPath()
{
    mDestinationPath.clear();
    mDestinationTargets.clear();
    if(!mDestinationBuilding) return;
    const auto src = mDestinationBuilding.get();
    // destination walkers cross roadblocks, so seed from blocked roads too.
    const auto srcRoads = src->surroundingRoad(false, true, true);
    if(srcRoads.empty()) return;
    const auto seedTile = srcRoads.front();
    const auto srcCid = src->cityId();
    const auto& targets = src->targets();
    const int w = mBoard->width();
    const int h = mBoard->height();
    for(const auto& tg : targets) {
        const auto targetType = tg.second;
        // pass 1: gather every reachable target with its road distance and path
        struct eReachable {
            eBuilding* fBldg;
            int fDist;
            std::vector<eTile*> fPath;
        };
        std::vector<eReachable> reachable;
        for(const auto b : mBoard->buildings()) {
            if(!b) continue;
            if(b->type() != targetType) continue;
            if(b->cityId() != srcCid) continue;
            const auto destBldg = b;
            const auto valid = [destBldg](eTileBase* const t) {
                const auto type = t->underBuildingType();
                if(type == eBuildingType::road) return true;
                if(type == eBuildingType::avenue) return true;
                const auto tt = static_cast<eTile*>(t);
                return tt->underBuilding() == destBldg;
            };
            const auto final = [destBldg](eTileBase* const t) {
                const auto tt = static_cast<eTile*>(t);
                return tt->underBuilding() == destBldg;
            };
            ePathFinder p(valid, final);
            const bool r = p.findPath({0, 0, w, h}, seedTile, 200, true, w, h,
                                      eWalkableHelpers::sRoadAvenueTileDistance);
            if(!r) continue;
            std::vector<eTile*> path;
            p.extractPath(path, *mBoard);
            ePathFindData data;
            p.extractData(data); // swaps out mData, so must come after extractPath
            reachable.push_back({destBldg, data.fDistance, std::move(path)});
        }
        if(reachable.empty()) continue;
        // pass 2: pick target using road distance (BFS) + load penalty.
        int bestI = -1;
        int bestScore = -1;
        for(int i = 0; i < (int)reachable.size(); i++) {
            const auto& re = reachable[i];
            int penalty = 0;
            const auto pt = dynamic_cast<ePatrolTarget*>(re.fBldg);
            if(pt) penalty = ePatrolSourceBuilding::sLoadPenalty(pt->showDays());
            const int score = re.fDist + penalty;
            if(bestScore < 0 || score < bestScore) { bestScore = score; bestI = i; }
        }
        if(bestI < 0) continue;
        const auto& re = reachable[bestI];
        mDestinationTargets.push_back(re.fBldg);
        for(const auto pt : re.fPath) {
            if(pt->underBuilding() == re.fBldg) continue;
            mDestinationPath.emplace_back(pt);
        }
    }
}

void GameWidget::setDestinationBuilding(ePatrolSourceBuilding* const sb)
{
    mDestinationBuilding = sb;
    mDestPathLastDay = -1;
    updateDestinationPath();
}

void GameWidget::tickDestinationPath(const int time)
{
    const int dayLen = eNumbers::sDayLength;
    const int curDay = dayLen > 0 ? time / dayLen : time;
    if(curDay == mDestPathLastDay) return;
    mDestPathLastDay = curDay;
    updateDestinationPath();
}
