#ifndef SANCTUARY_PREVIEW_H
#define SANCTUARY_PREVIEW_H

#include "engine/eworlddirection.h"

#include <SDL2/SDL_rect.h>

class eTerrainTextures;
class eTilePainter;
class GameBoard;

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eTerrainTextures& trrTexs,
    const SDL_Rect footprint,
    eWorldDirection dir,
    int boardw,
    int boardh,
    bool canBuild);

#endif // SANCTUARY_PREVIEW_H
