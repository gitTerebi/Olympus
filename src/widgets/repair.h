#ifndef REPAIR_H
#define REPAIR_H

#include "engine/game-board.h"
#include "buildings/ebuilding.h"

class GameWidget;

void handleRepair(GameBoard& board, GameWidget* const widget,
                  const int minX, const int minY,
                  const int maxX, const int maxY,
                  const eCityId cid,
                  const bool editorMode);

#endif // REPAIR_H
