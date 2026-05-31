#ifndef EPATHFINDER_H
#define EPATHFINDER_H

#include "epathfinderbase.h"

class ePathFinder : public ePathFinderBase {
public:
    using eTileFinish = std::function<bool(eTileBase* const)>;
    ePathFinder(const eTileWalkable& walkable,
                const eTileFinish& finish);

    using eTileDistance = std::function<int(eTileBase* const)>;
    bool findPath(const SDL_Rect& tileBRect,
                  eTileBase* const startTile,
                  const int maxDist,
                  const bool onlyDiagonal,
                  const int srcW, const int srcH,
                  const eTileDistance& distance = nullptr) override;
private:
    const eTileFinish mFinish;
};

#endif // EPATHFINDER_H
