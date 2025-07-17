#include "epathfinder.h"

#include <deque>
#include <map>

#include "engine/etilebase.h"
#include "engine/egameboard.h"

ePathFinder::ePathFinder(const eTileWalkable& walkable,
                         const eTileFinish& finish) :
    ePathFinderBase(walkable),
    mFinish(finish) {}

bool ePathFinder::findPath(const SDL_Rect& tileBRect,
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

    if(finishOnFound && mFinish(start)) {
        mData.fFound = true;
        mData.fDistance = 0;
        mData.fFinalX = start->x();
        mData.fFinalY = start->y();
        return true;
    }

    auto& brd = mData.fBoard;
    initializeBoard(brd, tileBRect, start, maxDist);

    std::deque<eTilePair> toProcess;
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
        if(mFinish(tt.first)) {
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
            toProcess.push_back(tt);
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
    toProcess.push_back(t);
    while((!mData.fFound || !finishOnFound) && !toProcess.empty()) {
        auto t = toProcess.front();
        toProcess.pop_front();
        pathFinder(t);
    }
    return mData.fFound;
}
