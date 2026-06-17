#include "widgets/paint/sanctuary-real-draw.h"

#include "buildings/allbuildings.h"
#include "engine/game-board.h"
#include "engine/etile.h"
#include "texture.h"
#include "textures/game-textures.h"

#include <algorithm>
#include "textures/building-textures.h"
#include "widgets/etilepainter.h"
#include "widgets/paint/sanctuary-preview.h"

namespace {

void drawTintedTexture(
    eTilePainter& tilePainter,
    const std::shared_ptr<Texture>& texture,
    const double drawX,
    const double drawY,
    const Alignment alignment,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue)
{
    if(!texture) return;
    if(colorMod) {
        texture->setColorMod(colorModRed, colorModGreen, colorModBlue);
    }
    tilePainter.drawTexture(drawX, drawY, texture, alignment);
    if(colorMod) {
        texture->clearColorMod();
    }
}

void drawAtTextureAnchor(
    eTilePainter& tilePainter,
    const eTextureSpace& textureSpace,
    const std::shared_ptr<Texture>& texture,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue)
{
    drawTintedTexture(
        tilePainter, texture,
        buildingDrawX + textureSpace.fX,
        buildingDrawY + textureSpace.fY,
        Alignment::top,
        colorMod, colorModRed, colorModGreen, colorModBlue);
}

void drawTemple(
    GameBoard& board,
    eTilePainter& tilePainter,
    const BuildingTextures& buildingTextures,
    eTempleBuilding& templeBuilding,
    const eTextureSpace& textureSpace,
    const eWorldDirection dir,
    const int animFrame,
    eTile* fallbackAltitudeTile,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue,
    const SanctuaryDrawXY& drawXY,
    std::vector<eMonument*>& drawnTempleWoman)
{
    const int stage = templeBuilding.progress() - 1;
    if(stage < 0) return;

    const int templeRotateId =
        templeBuilding.monument() ?
        templeBuilding.monument()->rotateId() : 0;
    const bool templeFrontVisible =
        sanctuaryTempleFrontFacing(templeRotateId, dir);

    GameTextures::loadZeusSanctuary();
    const auto textures = sanctuaryTempleGetTextures(
        buildingTextures, templeRotateId, dir, animFrame, stage);
    drawAtTextureAnchor(
        tilePainter, textureSpace, textures.fBase,
        buildingDrawX, buildingDrawY,
        colorMod, colorModRed, colorModGreen, colorModBlue);
    drawAtTextureAnchor(
        tilePainter, textureSpace, textures.fFlip,
        buildingDrawX, buildingDrawY,
        colorMod, colorModRed, colorModGreen, colorModBlue);

    if(!textures.fWoman || !templeFrontVisible) return;

    auto* monument = templeBuilding.monument();
    if(monument &&
       std::find(drawnTempleWoman.begin(), drawnTempleWoman.end(), monument) !=
           drawnTempleWoman.end()) {
        return;
    }
    if(monument) drawnTempleWoman.push_back(monument);

    const auto& templeWorldRect = templeBuilding.tileRect();
    double womanDrawX;
    double womanDrawY;
    const auto womanTile = board.tile(
        templeWorldRect.x, templeWorldRect.y);
    const int fallbackAltitude =
        fallbackAltitudeTile ? fallbackAltitudeTile->altitude() : 0;
    const int womanAltitude =
        womanTile ? womanTile->altitude() : fallbackAltitude;
    drawXY(templeWorldRect.x, templeWorldRect.y,
           womanDrawX, womanDrawY,
           1, 1, womanAltitude);
    const int womanCameraDirIdx = static_cast<int>(dir);
    womanDrawX += sanctuaryWomanTileDX(
        templeRotateId, womanCameraDirIdx);
    womanDrawY += sanctuaryWomanTileDY(
        templeRotateId, womanCameraDirIdx);

    drawTintedTexture(
        tilePainter, textures.fWoman,
        womanDrawX, womanDrawY,
        Alignment::bottom,
        colorMod, colorModRed, colorModGreen, colorModBlue);
}

void drawStatue(
    eTilePainter& tilePainter,
    const BuildingTextures& buildingTextures,
    eTempleStatueBuilding& statueBuilding,
    const eTextureSpace& textureSpace,
    const eWorldDirection dir,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue)
{
    if(statueBuilding.progress() <= 0) return;
    const int rotateId =
        statueBuilding.monument() ?
        statueBuilding.monument()->rotateId() : 0;
    drawAtTextureAnchor(
        tilePainter, textureSpace,
        sanctuaryStatueGetTexture(
            buildingTextures, statueBuilding.godType(), rotateId, dir),
        buildingDrawX, buildingDrawY,
        colorMod, colorModRed, colorModGreen, colorModBlue);
}

void drawMonument(
    eTilePainter& tilePainter,
    const BuildingTextures& buildingTextures,
    eTempleMonumentBuilding& monumentBuilding,
    const eTextureSpace& textureSpace,
    const eWorldDirection dir,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue)
{
    if(monumentBuilding.progress() <= 0) return;
    const int rotateId =
        monumentBuilding.monument() ?
        monumentBuilding.monument()->rotateId() : 0;
    drawAtTextureAnchor(
        tilePainter, textureSpace,
        sanctuaryMonumentGetTexture(
            buildingTextures, monumentBuilding.godType(), rotateId, dir),
        buildingDrawX, buildingDrawY,
        colorMod, colorModRed, colorModGreen, colorModBlue);
}

void drawAltar(
    eTilePainter& tilePainter,
    const BuildingTextures& buildingTextures,
    eTempleAltarBuilding& altarBuilding,
    const eTextureSpace& textureSpace,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue)
{
    if(altarBuilding.progress() <= 0) return;
    const int rotateId =
        altarBuilding.monument() ?
        altarBuilding.monument()->rotateId() : 0;
    const auto altarTexture = sanctuaryAltarGetTexture(
        buildingTextures, rotateId);
    const double altarOffsetX = (rotateId % 2 == 1) ? 2.0 : 0.0;
    const double altarOffsetY = (rotateId % 2 == 1) ? -2.0 : 0.0;
    drawTintedTexture(
        tilePainter, altarTexture,
        buildingDrawX + textureSpace.fX + altarOffsetX,
        buildingDrawY + textureSpace.fY + altarOffsetY,
        Alignment::top,
        colorMod, colorModRed, colorModGreen, colorModBlue);
}

} // namespace

bool isSanctuaryRealDrawPart(const eBuildingType buildingType)
{
    return buildingType == eBuildingType::temple ||
           buildingType == eBuildingType::templeStatue ||
           buildingType == eBuildingType::templeMonument ||
           buildingType == eBuildingType::templeAltar;
}

void drawSanctuaryRealBuildingPart(
    GameBoard& board,
    eTilePainter& tilePainter,
    const BuildingTextures& buildingTextures,
    eBuilding* const building,
    const eBuildingType buildingType,
    const eTextureSpace& textureSpace,
    const eWorldDirection dir,
    const int animFrame,
    eTile* const fallbackAltitudeTile,
    const double buildingDrawX,
    const double buildingDrawY,
    const bool colorMod,
    const int colorModRed,
    const int colorModGreen,
    const int colorModBlue,
    const SanctuaryDrawXY& drawXY,
    std::vector<eMonument*>& drawnTempleWoman)
{
    if(!building) return;

    if(buildingType == eBuildingType::temple) {
        drawTemple(
            board, tilePainter, buildingTextures,
            *static_cast<eTempleBuilding*>(building),
            textureSpace, dir, animFrame, fallbackAltitudeTile,
            buildingDrawX, buildingDrawY,
            colorMod, colorModRed, colorModGreen, colorModBlue,
            drawXY, drawnTempleWoman);
    } else if(buildingType == eBuildingType::templeStatue) {
        drawStatue(
            tilePainter, buildingTextures,
            *static_cast<eTempleStatueBuilding*>(building),
            textureSpace, dir,
            buildingDrawX, buildingDrawY,
            colorMod, colorModRed, colorModGreen, colorModBlue);
    } else if(buildingType == eBuildingType::templeMonument) {
        drawMonument(
            tilePainter, buildingTextures,
            *static_cast<eTempleMonumentBuilding*>(building),
            textureSpace, dir,
            buildingDrawX, buildingDrawY,
            colorMod, colorModRed, colorModGreen, colorModBlue);
    } else if(buildingType == eBuildingType::templeAltar) {
        drawAltar(
            tilePainter, buildingTextures,
            *static_cast<eTempleAltarBuilding*>(building),
            textureSpace,
            buildingDrawX, buildingDrawY,
            colorMod, colorModRed, colorModGreen, colorModBlue);
    }
}
