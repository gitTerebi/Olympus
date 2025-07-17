#include "epathfinderbase.h"

#include "engine/egameboard.h"

ePathFinderBase::ePathFinderBase(const eTileWalkable& walkable) :
    mWalkable(walkable) {}

bool ePathFinderBase::extractPath(
        std::vector<eOrientation>& path) {
    if(!mData.fFound) return false;
    path.clear();
    path.reserve(mData.fDistance);
    return extractPath([&](const eNeigh& n) {
        path.emplace_back(n.first);
    });
}

bool ePathFinderBase::extractPath(
        std::vector<std::pair<int, int>>& path) {
    if(!mData.fFound) return false;
    path.clear();
    path.reserve(mData.fDistance);
    return extractPath([&](const eNeigh& n) {
        const auto t = n.second.first;
        path.emplace_back(std::pair<int, int>{t->x(), t->y()});
    });
}

bool ePathFinderBase::extractPath(
        std::vector<eTile*>& path, eGameBoard& board) {
    if(!mData.fFound) return false;
    path.clear();
    path.reserve(mData.fDistance);
    return extractPath([&](const eNeigh& n) {
        const auto t = n.second.first;
        path.emplace_back(board.tile(t->x(), t->y()));
    });
}

bool ePathFinderBase::extractPath(
        const ePathFunc& pathFunc) {
    auto& brd = mData.fBoard;
    using eBFinder = std::function<bool(const eTilePair&)>;
    eBFinder bestFinder;

    const auto start = mData.fStart;
    const int startX = start->x();
    const int startY = start->y();
    const int finalX = mData.fFinalX;
    const int finalY = mData.fFinalY;

    bestFinder = [&](const eTilePair& from) {
        if(!from.first || !from.second) return false;
        const auto tile = from.first;
        const int dist = *from.second;
        const int tx = tile->x();
        const int ty = tile->y();
        if(tx == startX && ty == startY) return true;

        const auto tr = tileGetter(brd, tile, 0, -1);
        const auto br = tileGetter(brd, tile, 1, 0);
        const auto bl = tileGetter(brd, tile, 0, 1);
        const auto tl = tileGetter(brd, tile, -1, 0);

        const bool od = mData.fOnlyDiagonal;
        const bool isFinal = tx == finalX && ty == finalY;
        std::vector<eNeigh> neighs{
                {eOrientation::bottomLeft, tr},
                {eOrientation::topLeft, br},
                {eOrientation::topRight, bl},
                {eOrientation::bottomRight, tl},
                {eOrientation::left,
                        (!isFinal && (od || !checkNotDiagonalWalkable(brd, 1, -1, &from))) ?
                            eTilePair{nullptr, nullptr} :
                            tileGetter(brd, tile, 1, -1)},
                {eOrientation::top,
                        (!isFinal && (od || !checkNotDiagonalWalkable(brd, 1, 1, &from))) ?
                            eTilePair{nullptr, nullptr} :
                            tileGetter(brd, tile, 1, 1)},
                {eOrientation::right,
                        (!isFinal && (od || !checkNotDiagonalWalkable(brd, -1, 1, &from))) ?
                            eTilePair{nullptr, nullptr} :
                            tileGetter(brd, tile, -1, 1)},
                {eOrientation::bottom,
                        (!isFinal && (od || !checkNotDiagonalWalkable(brd, -1, -1, &from))) ?
                            eTilePair{nullptr, nullptr} :
                            tileGetter(brd, tile, -1, -1)}};

        int distP = __INT_MAX__;
        eNeigh best{eOrientation::topRight, {nullptr, &distP}};
        for(const auto& n : neighs) {
            const auto& tp = n.second;
            if(!tp.first || !tp.second) continue;

            if(mWalkable(tp.first)) {
                const int ddist = *tp.second;
                if(ddist < dist && ddist < *best.second.second) {
                    best = n;
                }
            }
        }
        if(best.second.first) {
            pathFunc(best);
            return bestFinder(best.second);
        }
        return false;
    };

    const auto t = tileGetter(brd, mData.fStart,
                              finalX - startX,
                              finalY - startY);
    const bool r = bestFinder(t);
    return r;
}

bool ePathFinderBase::extractData(ePathFindData& data) {
    if(!mData.fFound) return false;
    std::swap(mData, data);
    return true;
}

void ePathFinderBase::initializeBoard(
            ePathBoard& brd,
            const SDL_Rect& tileBRect,
            eTileBase* const start,
            const int maxDist) {
    const int startX = start->dx();
    const int startY = start->dy();

    const int minX = std::max(tileBRect.x, startX - maxDist);
    const int minY = std::max(tileBRect.y, startY - maxDist);

    const int maxX = std::min(tileBRect.x + tileBRect.w - 1, startX + maxDist);
    const int maxY = std::min(tileBRect.y + tileBRect.h - 1, startY + maxDist);

    brd = ePathBoard(minX, minY, maxX - minX + 1, maxY - minY + 1);
}

bool ePathFinderBase::checkNotDiagonalWalkable(
            ePathBoard& brd,
            const int x, const int y,
            const eTilePair* const tile) {
    const auto checkWalkable = [&](const int dx, const int dy) {
        const auto ttt = tileGetter(brd, tile->first, dx, dy);
        if(ttt.first && ttt.second) {
            return mWalkable(ttt.first);
        }
        return false;
    };
    if(x == -1 && y == -1) { // top
        {
            const bool w = checkWalkable(0, -1);
            if(!w) return false;
        }
        {
            const bool w = checkWalkable(-1, 0);
            if(!w) return false;
        }
    } else if(x == -1 && y == 1) { // left
        {
            const bool w = checkWalkable(-1, 0);
            if(!w) return false;
        }
        {
            const bool w = checkWalkable(0, 1);
            if(!w) return false;
        }
    } else if(x == 1 && y == 1) { // bottom
        {
            const bool w = checkWalkable(1, 0);
            if(!w) return false;
        }
        {
            const bool w = checkWalkable(0, 1);
            if(!w) return false;
        }
    } else if(x == 1 && y == -1) { // right
        {
            const bool w = checkWalkable(1, 0);
            if(!w) return false;
        }
        {
            const bool w = checkWalkable(0, -1);
            if(!w) return false;
        }
    }
    return true;
}
