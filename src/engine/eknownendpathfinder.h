#ifndef EKNOWNENDPATHFINDER_H
#define EKNOWNENDPATHFINDER_H

#include "epathfinderbase.h"

class eKnownEndPathFinder : public ePathFinderBase {
public:
    using eTileFinish = std::function<bool(eTileBase* const)>;
    eKnownEndPathFinder(const eTileWalkable& walkable,
                        eTileBase* const endTile);

    using eTileDistance = std::function<int(eTileBase* const)>;
    bool findPath(const SDL_Rect& tileBRect,
                  eTileBase* const startTile,
                  const int maxDist,
                  const bool onlyDiagonal,
                  const int srcW, const int srcH,
                  const eTileDistance& distance = nullptr) override;
private:
    eTileBase* const mEndTile;
};

#endif // EKNOWNENDPATHFINDER_H
