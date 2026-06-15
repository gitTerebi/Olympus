#include "widgets/game-widget.h"

#include "textures/game-textures.h"
#include "engine/game-board.h"
#include "evectorhelpers.h"

#include <algorithm>
#include <functional>
#include <vector>

void GameWidget::scheduleConnectedTerrainUpdate(eTile *const startTile)
{
    std::vector<eTile *> tiles;
    std::function<bool(eTile *)> check;
    std::function<void(eTile *)> prcs;
    prcs = [&](eTile *const tile)
    {
        if (!tile)
            return;
        if (!check(tile))
            return;
        if (eVectorHelpers::contains(tiles, tile))
            return;
        tiles.push_back(tile);
        tile->scheduleTerrainUpdate();
        tile->setDrawDim(1);
        tile->setUnderTile(nullptr);
        const auto tr = tile->topRight<eTile>();
        prcs(tr);
        const auto br = tile->bottomRight<eTile>();
        prcs(br);
        const auto bl = tile->bottomLeft<eTile>();
        prcs(bl);
        const auto tl = tile->topLeft<eTile>();
        prcs(tl);
    };

    const auto terr = startTile->terrain();
    if (static_cast<bool>(terr & eTerrain::stones))
    {
        check = [terr](eTile *const tile)
        {
            return tile->terrain() == terr;
        };
        prcs(startTile);
    }
    else if (startTile->underBuildingType() == eBuildingType::park)
    {
        check = [](eTile *const tile)
        {
            return tile->underBuildingType() == eBuildingType::park;
        };
        prcs(startTile);
    }
    else
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                const auto t = startTile->tileRel<eTile>(dx, dy);
                if (!t)
                    continue;
                t->scheduleTerrainUpdate();
            }
        }
    }
    std::sort(tiles.begin(), tiles.end(),
              [this](eTile *const t1, eTile *const t2)
              {
                  const auto dir = mBoard->direction();
                  const int t1dx = t1->dx();
                  const int t1dy = t1->dy();
                  const int t2dx = t2->dx();
                  const int t2dy = t2->dy();
                  switch (dir)
                  {
                  case eWorldDirection::N:
                  {
                      if (t1dy != t2dy)
                          return t1dy < t2dy;
                      return t1dx < t2dx;
                  }
                  break;
                  case eWorldDirection::E:
                  {
                      if (t1dx != t2dx)
                          return t1dx < t2dx;
                      return t1dy < t2dy;
                  }
                  break;
                  case eWorldDirection::S:
                  {
                      if (t1dy != t2dy)
                          return t1dy > t2dy;
                      return t1dx > t2dx;
                  }
                  break;
                  case eWorldDirection::W:
                  {
                      if (t1dx != t2dx)
                          return t1dx > t2dx;
                      return t1dy > t2dy;
                  }
                  break;
                  }
              });

    const int tid = static_cast<int>(mTileSize);
    const auto &trrTexs = GameTextures::terrain().at(tid);
    const auto &builTexs = GameTextures::buildings().at(tid);

    for (const auto tile : tiles)
    {
        updateTerrainTextures(tile, trrTexs, builTexs);
    }
}
