#ifndef SANCTUARY_PREVIEW_H
#define SANCTUARY_PREVIEW_H

#include "engine/eworlddirection.h"
#include "characters/gods/egodtype.h"
#include "pointers/estdselfref.h"

#include <SDL2/SDL_rect.h>
#include <vector>

class eBuilding;
enum class eBuildingMode;
class eBuildingTextures;
class eTerrainTextures;
class eTilePainter;
class GameBoard;
enum class eCityId;

struct SanctuaryPreviewEntry
{
    SanctuaryPreviewEntry(int order,
                          int tx,
                          int ty,
                          int altitude,
                          int templeOverlayDirId,
                          eGodType statueGod,
                          int statueTextureId,
                          int monumentTextureId,
                          bool altar,
                          const stdsptr<eBuilding>& b);

    int fOrder;
    int fTx;
    int fTy;
    int fAltitude;
    int fTempleOverlayDirId;
    eGodType fStatueGod;
    int fStatueTextureId;
    int fMonumentTextureId;
    bool fAltar;
    bool fTorch = false;
    stdsptr<eBuilding> fB;
};

std::vector<SanctuaryPreviewEntry> createSanctuaryPreviewEntries(
    GameBoard& board,
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY,
    eCityId viewedCityId,
    SDL_Rect& footprint);

void printSanctuaryPreviewTiles(
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY);

void adjustSanctuaryTemplePreviewDebugOffset(
    eBuildingMode mode,
    int rotateId,
    int dx,
    int dy);

void drawSanctuaryTempleBuildingPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    int tx,
    int ty,
    int altitude,
    int templeOverlayDirId,
    eWorldDirection dir,
    int animFrame,
    bool canBuild);

void drawSanctuaryStatuePreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    eGodType god,
    int statueTextureId,
    int tx,
    int ty,
    int altitude,
    eWorldDirection dir,
    bool canBuild);

void drawSanctuaryMonumentPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    eGodType god,
    int monumentTextureId,
    int tx,
    int ty,
    int altitude,
    eWorldDirection dir,
    bool canBuild);

void drawSanctuaryAltarPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    int tx,
    int ty,
    int altitude,
    eWorldDirection dir,
    int rotateId,
    bool canBuild);

void drawSanctuaryTorchPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    int tx,
    int ty,
    int altitude,
    int animFrame);

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const eTerrainTextures& trrTexs,
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY,
    eCityId viewedCityId,
    const SDL_Rect footprint,
    eWorldDirection dir,
    int boardw,
    int boardh,
    int animFrame,
    bool canBuild);

#endif // SANCTUARY_PREVIEW_H
