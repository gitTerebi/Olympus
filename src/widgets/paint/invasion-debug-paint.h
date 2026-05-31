#ifndef INVASION_DEBUG_PAINT_H
#define INVASION_DEBUG_PAINT_H

#include "engine/ecityid.h"

class GameBoard;
class ePainter;

void paintInvasionDebugTargets(GameBoard& board,
                               const eCityId viewedCity,
                               ePainter& p,
                               const int tileW,
                               const int tileH,
                               const int animFrame);

#endif // INVASION_DEBUG_PAINT_H
