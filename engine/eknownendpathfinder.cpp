#include "eknownendpathfinder.h"

#include <deque>
#include <map>

#include "engine/etilebase.h"
#include "engine/egameboard.h"

eKnownEndPathFinder::eKnownEndPathFinder(
        const eTileWalkable& walkable,
        eTileBase* const endTile) :
    ePathFinderBase(walkable),
    mEndTile(endTile) {}

int manhattanDistance(const int x0, const int y0,
                      const int x1, const int y1) {
    return std::abs(x0 - x1) + std::abs(y0 - y1);
}

int manhattanDistance(const eTileBase* const t0,
                      const eTileBase* const t1) {
    return manhattanDistance(t0->x(), t0->y(),
                             t1->x(), t1->y());
}

bool eKnownEndPathFinder::findPath(
        const SDL_Rect& tileBRect,
        eTileBase* const start,
        const int maxDist,
        const bool onlyDiagonal,
        const int srcW, const int srcH,
        const eTileDistance& distance) {
    (void)srcW;
    (void)srcH;
    if(!start) return false;

    const bool finishOnFound = mMode == ePathFinderMode::findSingle;

    mData = ePathFindData();
    mData.fOnlyDiagonal = onlyDiagonal;
    mData.fStart = start;

    if(finishOnFound && mEndTile == start) {
        mData.fFound = true;
        mData.fDistance = 0;
        mData.fFinalX = start->x();
        mData.fFinalY = start->y();
        return true;
    }

    auto& brd = mData.fBoard;
    initializeBoard(brd, tileBRect, start, maxDist);

    std::map<int, std::vector<eTilePair>> toProcess;
    const auto processTile = [&](eTilePair* const tile,
                                 const int x, const int y,
                                 const int dist) {
        if(x == 0 && y == 0) return;
        const bool notDiagonal = x != 0 && y != 0;
        if(onlyDiagonal && notDiagonal) return;
        auto tt = tileGetter(brd, tile->first, x, y);
        if(!tt.first || !tt.second) return;
        if(tt.first->cityId() != tile->first->cityId()) return;
        const int dinc = distance ? distance(tt.first) : 1;
        const int newDist = dist + dinc;
        if(mEndTile == tt.first) {
            *tt.second = newDist;
            const int ttx = tt.first->x();
            const int tty = tt.first->y();
            mData.fFound = true;
            mData.fDistance = newDist;
            mData.fFinalX = ttx;
            mData.fFinalY = tty;
            if(finishOnFound) return;
        }
        if(!mWalkable(tt.first)) return;
        if(notDiagonal) {
            const bool r = checkNotDiagonalWalkable(brd, x, y, tile);
            if(!r) return;
        }
        if(*tt.second > newDist) {
            *tt.second = newDist;
            toProcess[manhattanDistance(mEndTile, tt.first)].push_back(tt);
        }
    };
    const auto pathFinder = [&](eTilePair& from) {
        if(!from.first || !from.second) return;
        const int dist = *from.second;
        if(dist > maxDist) return;

        for(const int x : {0, 1, -1}) {
            for(const int y : {0, 1, -1}) {
                processTile(&from, x, y, dist);
            }
        }
    };

    const auto t = tileGetter(brd, start, 0, 0);
    if(!t.first || !t.second) return false;
    *t.second = 0;
    toProcess[manhattanDistance(mEndTile, t.first)].push_back(t);
    while((!mData.fFound || !finishOnFound) && !toProcess.empty()) {
        const auto it = toProcess.begin();
        auto& vec = it->second;
        auto t = vec.back();
        vec.pop_back();
        if(vec.empty()) toProcess.erase(it);
        pathFinder(t);
    }
    return mData.fFound;
}
