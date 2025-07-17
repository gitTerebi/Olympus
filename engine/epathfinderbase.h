#ifndef EPATHFINDERBASE_H
#define EPATHFINDERBASE_H

#include <functional>
#include <SDL2/SDL_rect.h>

#include "eorientation.h"
#include "epathboard.h"

#include "engine/etilebase.h"

struct ePathFindData {
    ePathBoard fBoard;

    eTileBase* fStart = nullptr;
    bool fOnlyDiagonal = false;

    bool fFound = false;
    int fDistance = -1;
    int fFinalX = -1;
    int fFinalY = -1;
};

enum class ePathFinderMode {
    findSingle,
    findAll
};

using eTilePair = std::pair<eTileBase*, int*>;
using eNeigh = std::pair<eOrientation, eTilePair>;

inline eTilePair tileGetter(ePathBoard& brd,
                            eTileBase* const from,
                            const int dx, const int dy) {
    const auto tile = from->tileRel<eTileBase>(dx, dy);
    if(!tile) return eTilePair{nullptr, nullptr};
    const int tx = tile->x();
    const int ty = tile->y();
    int* value = nullptr;
    const bool r = brd.getAbsValue(tx, ty, &value);
    if(!r) return eTilePair{nullptr, nullptr};
    return eTilePair{tile, value};
};

class ePathFinderBase {
public:
    using eTileWalkable = std::function<bool(eTileBase* const)>;
    ePathFinderBase(const eTileWalkable& walkable);

    using eTileDistance = std::function<int(eTileBase* const)>;
    virtual bool findPath(const SDL_Rect& tileBRect,
                          eTileBase* const startTile,
                          const int maxDist,
                          const bool onlyDiagonal,
                          const int srcW, const int srcH,
                          const eTileDistance& distance = nullptr) = 0;

    bool extractPath(std::vector<eOrientation>& path);
    bool extractPath(std::vector<std::pair<int, int>>& path);
    bool extractPath(std::vector<eTile*>& path,
                     eGameBoard& board);
    using ePathFunc = std::function<void(const eNeigh&)>;
    bool extractPath(const ePathFunc& pathFunc);
    bool extractData(ePathFindData& data);

    void setMode(const ePathFinderMode m)
    { mMode = m; }
protected:
    void initializeBoard(ePathBoard& brd,
                         const SDL_Rect& tileBRect,
                         eTileBase* const start,
                         const int maxDist);
    bool checkNotDiagonalWalkable(ePathBoard& brd,
                                  const int x, const int y,
                                  eTilePair* const tile);

    const eTileWalkable mWalkable;
    ePathFinderMode mMode = ePathFinderMode::findSingle;
    ePathFindData mData;
};

#endif // EPATHFINDERBASE_H
