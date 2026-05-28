#include "widgets/game-widget.h"

#include "engine/e-game-board.h"
#include "engine/etile.h"
#include "engine/epathfinder.h"
#include "buildings/epatrolbuildingbase.h"
#include "buildings/epatrolsourcebuilding.h"
#include "characters/actions/walkable/ewalkableobject.h"

using ePatrolGuides = std::vector<ePatrolGuide>;

void GameWidget::updatePatrolPath()
{
    if (!mPatrolBuilding)
    {
        mPatrolPath.clear();
        mExcessPatrolPath.clear();
        mPatrolPath1.clear();
        mExcessPatrolPath1.clear();
        return;
    }
    const bool r = mPatrolBuilding->updatePath([this]()
                                               {
        mPatrolPath.clear();
        mExcessPatrolPath.clear();
        mPatrolPath1.clear();
        mExcessPatrolPath1.clear();
        if(!mPatrolBuilding) return;
        const auto startTile = mPatrolBuilding->patrolStartTile();
        {
            const auto& path = mPatrolBuilding->path();
            auto lastTile = startTile;
            mPatrolPath.push_back(lastTile);
            const int iMin = path.size() - 1;
            for(int i = iMin; i >= 0; i--) {
                const auto o = path[i];
                lastTile = lastTile->neighbour<eTile>(o);
                mPatrolPath.push_back(lastTile);
            }
        }
        {
            const auto& path = mPatrolBuilding->reversePath();
            auto lastTile = startTile;
            mPatrolPath1.push_back(lastTile);
            const int iMin = path.size() - 1;
            for(int i = iMin; i >= 0; i--) {
                const auto o = path[i];
                lastTile = lastTile->neighbour<eTile>(o);
                mPatrolPath1.push_back(lastTile);
            }
        }

        const auto handlePatrolPath = [&](std::vector<eTile*>& excessPath,
                                          const ePatrolGuides& guides) {
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
            for(const auto& g : guides) {
                if(!lastTile) break;
                const auto guideTile = mBoard->tile(g.fX, g.fY);
                if(!guideTile) break;
                const bool r = handlePath(lastTile, guideTile);
                if(!r) break;
                lastTile = guideTile;
            }
        };
        {
            const auto& guides = mPatrolBuilding->patrolGuides();
            handlePatrolPath(mExcessPatrolPath, guides);
        }
        if(mPatrolBuilding->bothDirections()) {
            const auto guides = mPatrolBuilding->reversePatrolGuides();
            handlePatrolPath(mExcessPatrolPath1, guides);
        } });
    if (!r)
    {
        mPatrolPath.clear();
        mExcessPatrolPath.clear();
        mPatrolPath1.clear();
        mExcessPatrolPath1.clear();
    }
}

void GameWidget::updateDestinationPath()
{
    mDestinationPath.clear();
    mDestinationTargets.clear();
    if(!mDestinationBuilding) return;
    const auto src = mDestinationBuilding.get();
    const auto srcRoads = src->surroundingRoad(false, true);
    if(srcRoads.empty()) return;
    const auto seedTile = srcRoads.front();
    const auto srcCid = src->cityId();
    const auto& targets = src->targets();
    const int w = mBoard->width();
    const int h = mBoard->height();
    for(const auto& tg : targets) {
        const auto targetType = tg.second;
        for(const auto b : mBoard->buildings()) {
            if(!b) continue;
            if(b->type() != targetType) continue;
            if(b->cityId() != srcCid) continue;
            mDestinationTargets.push_back(b);
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
            for(const auto pt : path) {
                if(pt->underBuilding() == destBldg) continue;
                mDestinationPath.emplace_back(pt);
            }
        }
    }
}

void GameWidget::setDestinationBuilding(ePatrolSourceBuilding* const sb)
{
    mDestinationBuilding = sb;
    updateDestinationPath();
}
