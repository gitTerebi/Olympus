#ifndef SANCTUARY_PREVIEW_H
#define SANCTUARY_PREVIEW_H

#include "engine/eworlddirection.h"
#include "characters/gods/god-type.h"
#include "pointers/estdselfref.h"

#include <SDL2/SDL_rect.h>
#include <memory>
#include <vector>

class eBuilding;
class eTexture;
enum class eBuildingMode;
enum class eBuildingType;
class BuildingTextures;
class TerrainTextures;
class eTilePainter;
class GameBoard;
enum class eCityId;

struct SanctuaryPreviewEntry
{
    SanctuaryPreviewEntry(int order,
                          int worldTileX,
                          int worldTileY,
                          int altitude,
                          int templeOverlayDirId,
                          GodType statueGod,
                          int statueTextureId,
                          int monumentTextureId,
                          bool altar,
                          const stdsptr<eBuilding> &b);

    int fOrder;
    int fTx;
    int fTy;
    int fAltitude;
    int fTempleOverlayDirId;
    GodType fStatueGod;
    int fStatueTextureId;
    int fMonumentTextureId;
    bool fAltar;
    bool fTorch = false;
    stdsptr<eBuilding> fB;
};

std::vector<SanctuaryPreviewEntry> createSanctuaryPreviewEntries(
    GameBoard &board,
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY,
    eCityId viewedCityId,
    SDL_Rect &footprint);

void printSanctuaryPreviewTiles(
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY);

void adjustSanctuaryTemplePreviewDebugOffset(
    eBuildingMode mode,
    int rotateId,
    int dirIdx,
    int dx,
    int dy);

void adjustWomanDebugOffset(
    int rotateId,
    int dirIdx,
    int dx,
    int dy);

double sanctuaryWomanTileDX(int rotateId, int dirIdx);
double sanctuaryWomanTileDY(int rotateId, int dirIdx);

struct SanctuaryTempleTextures {
    std::shared_ptr<eTexture> fBase;
    std::shared_ptr<eTexture> fFlip;
    std::shared_ptr<eTexture> fWoman;
};

SanctuaryTempleTextures sanctuaryTempleGetTextures(
    const BuildingTextures& builTexs,
    int rotateId,
    eWorldDirection dir,
    int animFrame,
    int stage);

std::shared_ptr<eTexture> sanctuaryStatueGetTexture(
    const BuildingTextures& builTexs,
    GodType god,
    int rotateId,
    eWorldDirection dir);

std::shared_ptr<eTexture> sanctuaryMonumentGetTexture(
    const BuildingTextures& builTexs,
    GodType god,
    int rotateId,
    eWorldDirection dir);

std::shared_ptr<eTexture> sanctuaryAltarGetTexture(
    const BuildingTextures& builTexs,
    int rotateId);

void sanctuaryTempleDrawOrigin(
    eBuildingType type,
    int rotateId,
    int xMin,
    int yMin,
    int& outTx,
    int& outTy);


void drawSanctuaryTempleBuildingPreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    int worldTileX,
    int worldTileY,
    int altitude,
    int placementRotateId,
    eWorldDirection dir,
    int animFrame,
    bool canBuild,
    int stage = 2);

void drawSanctuaryStatuePreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    GodType god,
    int statueTextureId,
    int worldTileX,
    int worldTileY,
    int altitude,
    eWorldDirection dir,
    bool canBuild);

void drawSanctuaryMonumentPreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    GodType god,
    int monumentTextureId,
    int worldTileX,
    int worldTileY,
    int altitude,
    eWorldDirection dir,
    bool canBuild);

void drawSanctuaryAltarPreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    int worldTileX,
    int worldTileY,
    int altitude,
    eWorldDirection dir,
    int rotateId,
    bool canBuild);

void drawSanctuaryTorchPreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    int worldTileX,
    int worldTileY,
    int altitude,
    int animFrame);

void drawSanctuaryTerrainPreview(
    GameBoard &board,
    eTilePainter &tp,
    const BuildingTextures &builTexs,
    const TerrainTextures &trrTexs,
    eBuildingMode mode,
    int rotateId,
    int hoverTX,
    int hoverTY,
    eCityId viewedCityId,
    const SDL_Rect footprint,
    eWorldDirection dir,
    int boardWidth,
    int boardHeight,
    int animFrame,
    bool canBuild);

#endif // SANCTUARY_PREVIEW_H
