#include "epatrolsourcebuilding.h"

#include <memory>
#include <cmath>

#include "textures/game-textures.h"

#include "characters/actions/emovetoaction.h"
#include "engine/epathfinder.h"
#include "path-find-task.h"
#include "engine/ethreadpool.h"
#include "engine/game-board.h"
#include "characters/actions/walkable/walkable-object.h"
#include "vector-helpers.h"
#include "epatroltarget.h"
#include "estadium.h"
#include "emuseum.h"
#include "fileIO/save-archive.h"

ePatrolSourceBuilding::ePatrolSourceBuilding(GameBoard &board,
                                             const eBaseTex baseTex,
                                             const double overlayX,
                                             const double overlayY,
                                             const eOverlays overlays,
                                             const eTargets &targets,
                                             const eBuildingType type,
                                             const int sw, const int sh,
                                             const int maxEmployees,
                                             const eCityId cid,
                                             const eCharGenerator &charGen) : ePatrolBuilding(board, baseTex, overlayX, overlayY,
                                                                                              overlays, charGen, type, sw, sh, maxEmployees, cid),
                                                                              mTargets(targets)
{
    mSpawnInterval = Numbers::sDestinationWalkerRecurringSpawnDays * Numbers::sDayLength;
    mInitialDelay = Numbers::sDestinationWalkerInitialSpawnWaitDays * Numbers::sDayLength;
    for (const auto &t : mTargets)
    {
        (void)t;
        mTargetData.push_back({mSpawnInterval - mInitialDelay, 0, nullptr});
    }
}

void ePatrolSourceBuilding::timeChanged(const int by)
{
    ePatrolBuilding::timeChanged(by);
    if (enabled())
    {
        const int me = maxEmployees();
        const int em = employed();
        const int scaledBy = (me > 0) ? (by * em) / me : 0;
        const int iMax = mTargetData.size();
        const int dayLen = Numbers::sDayLength;
        for (int i = 0; i < iMax; i++)
        {
            int &spawnTime = mTargetData[i].fSpawnTime;
            spawnTime += scaledBy;
            if (spawnTime > mSpawnInterval)
            {
                spawnTime = 0;
                spawn(i);
            }
            if (targetWalkerInFlight(i) && dayLen > 0)
            {
                int &rerouteAccum = mTargetData[i].fRerouteAccum;
                rerouteAccum += by;
                if (rerouteAccum >= dayLen)
                {
                    rerouteAccum -= dayLen;
                    reroute(i);
                }
            }
            else
            {
                mTargetData[i].fRerouteAccum = 0;
            }
        }
    }
}

void ePatrolSourceBuilding::serializeFields(SaveArchive &ar)
{
    ePatrolBuildingBase::serializeFields(ar);
    if (ar.reading())
    {
        const stdptr<ePatrolSourceBuilding> tptr(this);
        auto targetData = std::make_shared<std::vector<eTargetData>>(mTargetData);
        ar.fixedArrayField("targetData", *targetData,
                           [this](SaveArchive &itemAr, eTargetData &td)
                           {
                               itemAr.field("fSpawnTime", td.fSpawnTime);
                               itemAr.field("fRerouteAccum", td.fRerouteAccum);
                               itemAr.characterField("walker", &getBoard(), td.fWalker);
                           });
        ar.addPostFunc([tptr, targetData]()
                       {
            if(!tptr) return;
            tptr->mTargetData = *targetData; }, "ePatrolSourceBuilding::targetData");
    }
    else
    {
        ar.fixedArrayField("targetData", mTargetData,
                           [this](SaveArchive &itemAr, eTargetData &td)
                           {
                               itemAr.field("fSpawnTime", td.fSpawnTime);
                               itemAr.field("fRerouteAccum", td.fRerouteAccum);
                               itemAr.characterField("walker", &getBoard(), td.fWalker);
                           });
    }
}

bool ePatrolSourceBuilding::targetWalkerInFlight(const int id) const
{
    if (id < 0 || id >= static_cast<int>(mTargets.size()))
        return false;
    const auto c = mTargetData[id].fWalker.get();
    if (!c)
        return false;
    const auto a = c->action();
    if (!a)
        return false;
    return a->state() == eCharacterActionState::running;
}

bool operator==(const SDL_Rect &r1, const SDL_Rect &r2)
{
    return r1.x == r2.x && r1.y == r2.y &&
           r1.w == r2.w && r1.h == r2.h;
}

int ePatrolSourceBuilding::sBuildingScore(int fromX, int fromY, eBuilding* b)
{
    const auto r = b->tileRect();
    const long dx = (r.x + r.w / 2) - fromX;
    const long dy = (r.y + r.h / 2) - fromY;
    const int dist = (int)(std::sqrt((double)(dx * dx + dy * dy)) + 0.5);
    const auto pt = dynamic_cast<ePatrolTarget*>(b);
    return dist + sLoadPenalty(pt ? pt->showDays() : 0);
}

int ePatrolSourceBuilding::sLoadPenalty(const int showDays)
{
    // Augustus penalizes a venue by 2 * remaining show-days so the next
    // walker prefers an idle venue over a recently-served one.
    if (showDays <= 0)
        return 0;
    return showDays * sLoadBias;
}

void ePatrolSourceBuilding::spawn(const int id)
{
    auto &board = getBoard();
    const auto cid = cityId();
    const auto &target = mTargets[id];
    const auto targetType = target.second;
    if (targetType == eBuildingType::stadium)
    {
        const auto s = board.stadium(cid);
        if (!s)
            return;
        return spawn(id, s);
    }
    else if (targetType == eBuildingType::museum)
    {
        const auto m = board.museum(cid);
        if (!m)
            return;
        return spawn(id, m);
    }

    const auto t = centerTile();
    auto &tp = board.threadPool();

    const int tx = t->x();
    const int ty = t->y();

    const auto startTile = [tx, ty](eThreadBoard &board)
    {
        return board.tile(tx, ty);
    };
    using eTargetRect = std::pair<SDL_Rect, int>; // rect + road distance
    using eTargetRects = std::vector<eTargetRect>;
    const auto targetRects = std::make_shared<eTargetRects>();
    const auto tRect = tileRect();
    const auto finalTile = [targetType, targetRects](eThreadTile *const t)
    {
        const auto &ub = t->underBuilding();
        const auto ubt = ub.type();
        const bool found = ubt == targetType;
        if (found)
        {
            const auto &ubr = ub.tileRect();
            bool c = false;
            for (const auto &tr : *targetRects)
            {
                if (tr.first == ubr)
                {
                    c = true;
                    break;
                }
            }
            if (!c)
                targetRects->push_back({ubr, -1});
        }
        return found;
    };
    // road distance per matched tile -> fold into each target building's
    // shortest distance (the rect that contains the tile).
    const auto foundFunc = [targetRects](const PathFindTask::eFoundTiles &found)
    {
        for (const auto &f : found)
        {
            const int fx = f.first.first;
            const int fy = f.first.second;
            const int dist = f.second;
            for (auto &tr : *targetRects)
            {
                const auto &r = tr.first;
                const bool in = fx >= r.x && fx < r.x + r.w &&
                                fy >= r.y && fy < r.y + r.h;
                if (!in)
                    continue;
                if (tr.second < 0 || dist < tr.second)
                    tr.second = dist;
                break;
            }
        }
    };
    const auto failFunc = []() {};
    const auto rw = WalkableObject::sCreateRoadAvenue();
    const auto walkable = WalkableObject::sCreateRect(tRect, rw);
    using ePath = std::vector<eOrientation>;
    const stdptr<ePatrolSourceBuilding> tptr(this);
    const auto finishFunc = [tptr, walkable, targetRects, id, tx, ty](const ePath &)
    {
        if (!tptr)
            return;
        if (targetRects->empty())
            return;
        auto &board = tptr->getBoard();
        // Augustus model: score = straight-line distance + load penalty.
        // a recently-served target is penalized so the next walker prefers a
        // farther idle target. pick the single lowest score, no round-robin.
        eBuilding *best = nullptr;
        int bestScore = -1;
        for (const auto &tr : *targetRects)
        {
            const auto &r = tr.first;
            const auto targetTile = board.tile(r.x, r.y);
            if (!targetTile) continue;
            const auto tb = targetTile->underBuilding();
            if (!tb) continue;
            const int score = sBuildingScore(tx, ty, tb);
            if (bestScore < 0 || score < bestScore) { bestScore = score; best = tb; }
        }
        if (!best)
            return;
        tptr->spawn(id, best);
    };

    const auto tileBRect = board.boardCityTileBRect(cid);
    const auto pft = new PathFindTask(cid, tileBRect,
                                       startTile, walkable,
                                       finalTile, finishFunc,
                                       failFunc, true, 200,
                                       nullptr, nullptr, true);
    pft->setFoundFunc(foundFunc);
    tp.queueTask(pft);
}

void ePatrolSourceBuilding::reroute(const int id)
{
    const auto c = mTargetData[id].fWalker.get();
    if (!c) return;
    const auto ma = dynamic_cast<eMoveToAction*>(c->action());
    if (!ma) return;
    const auto fa = dynamic_cast<ePT_spawnGetActorFinish*>(ma->finishAction());
    if (!fa) return;
    ePatrolTarget* const currentTarget = fa->target();

    const auto &target = mTargets[id];
    const auto targetType = target.second;
    if (targetType == eBuildingType::stadium || targetType == eBuildingType::museum)
        return;

    const auto walkerTile = c->tile();
    if (!walkerTile) return;
    const int wx = walkerTile->x();
    const int wy = walkerTile->y();

    auto &board = getBoard();
    const auto cid = cityId();
    eBuilding *best = nullptr;
    int bestScore = -1;
    for (const auto b : board.buildings())
    {
        if (!b || b->type() != targetType || b->cityId() != cid) continue;
        const int score = sBuildingScore(wx, wy, b);
        if (bestScore < 0 || score < bestScore) { bestScore = score; best = b; }
    }
    if (!best || best == currentTarget) return;
    const auto newTarget = dynamic_cast<ePatrolTarget*>(best);
    if (!newTarget) return;
    const auto newFinish = std::make_shared<ePT_spawnGetActorFinish>(board, newTarget);
    ma->setFinishAction(newFinish);
    ma->start(best, WalkableObject::sCreateRoadAvenue());
}

void ePatrolSourceBuilding::spawn(const int id, eBuilding *const targetBuilding)
{
    const auto patrolTarget = dynamic_cast<ePatrolTarget *>(targetBuilding);
    if (!patrolTarget)
        return spawn(id);

    const auto &target = mTargets[id];
    auto &board = getBoard();
    const auto c = eCharacter::sCreate(target.first, board);
    c->setBothCityIds(cityId());
    c->changeTile(centerTile());
    mTargetData[id].fWalker = c.get();

    const auto finishAction = std::make_shared<ePT_spawnGetActorFinish>(
        board, patrolTarget);

    const auto a = e::make_shared<eMoveToAction>(c.get());
    a->setStateRelevance(eStateRelevance::buildings);
    a->setFinishAction(finishAction);
    c->setAction(a);
    c->setActionType(eCharacterActionType::walk);
    a->start(targetBuilding, WalkableObject::sCreateRoadAvenue());
}
