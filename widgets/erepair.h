#ifndef EREPAIR_H
#define EREPAIR_H

#include "engine/e-game-board.h"
#include "buildings/ebuilding.h"

class eGameWidget;

void handleRepair(eGameBoard& board, eGameWidget* const widget,
                  const int minX, const int minY,
                  const int maxX, const int maxY,
                  const eCityId cid,
                  const bool editorMode);

#endif // EREPAIR_H
