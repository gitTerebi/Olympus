#ifndef SANCTUARY_PREVIEW_H
#define SANCTUARY_PREVIEW_H

#include "buildings/sanctuaries/esanctuaryblueprint.h"
#include "engine/eworlddirection.h"
#include "pointers/estdpointer.h"

#include <SDL2/SDL_rect.h>
#include <vector>

class eBuilding;
class eBuildingRenderer;
class eBuildingTextures;
class eTerrainTextures;
class eTilePainter;
class GameBoard;

struct SanctuaryPreviewCell
{
    int fX;
    int fY;
    int fId;
    int fAltitude;
    eSanctEleType fType;
};

struct SanctuaryPreviewBuilding
{
    int fX;
    int fY;
    stdsptr<eBuilding> fBuilding;
    stdsptr<eBuildingRenderer> fRenderer;
};

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eTerrainTextures& trrTexs,
    const eBuildingTextures& builTexs,
    const stdsptr<eBuilding>& sanctuary,
    const std::vector<SanctuaryPreviewCell>& cells,
    const std::vector<SanctuaryPreviewBuilding>& buildings,
    const SDL_Rect footprint,
    eWorldDirection dir,
    int boardw,
    int boardh,
    bool drawElevation,
    bool canBuild);

#endif // SANCTUARY_PREVIEW_H
