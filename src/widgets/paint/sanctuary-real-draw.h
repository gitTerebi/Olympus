#ifndef SANCTUARY_REAL_DRAW_H
#define SANCTUARY_REAL_DRAW_H

#include "engine/eworlddirection.h"

#include <functional>
#include <set>

class eBuilding;
class eBuildingTextures;
class eMonument;
class eTile;
class eTilePainter;
class GameBoard;
enum class eBuildingType;
struct eTextureSpace;

using SanctuaryDrawXY = std::function<void(
    int worldTileX,
    int worldTileY,
    double& drawX,
    double& drawY,
    int tileSpanW,
    int tileSpanH,
    int altitude)>;

bool isSanctuaryRealDrawPart(eBuildingType buildingType);

void drawSanctuaryRealBuildingPart(
    GameBoard& board,
    eTilePainter& tilePainter,
    const eBuildingTextures& buildingTextures,
    eBuilding* building,
    eBuildingType buildingType,
    const eTextureSpace& textureSpace,
    eWorldDirection dir,
    int animFrame,
    eTile* fallbackAltitudeTile,
    double buildingDrawX,
    double buildingDrawY,
    bool colorMod,
    int colorModRed,
    int colorModGreen,
    int colorModBlue,
    const SanctuaryDrawXY& drawXY,
    std::set<eMonument*>& drawnTempleWoman);

#endif // SANCTUARY_REAL_DRAW_H
