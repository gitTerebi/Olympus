#ifndef BUILD_PREVIEW_RENDER_H
#define BUILD_PREVIEW_RENDER_H

#include "engine/eworlddirection.h"

class eBuilding;
class eBuildingRenderer;
class eTile;
class eTilePainter;

void drawGenericBuildPreviewPart(
    eTilePainter& tp,
    eBuilding* building,
    eBuildingRenderer* renderer,
    eTile* centerTile,
    int tx,
    int ty,
    int altitude,
    eWorldDirection dir,
    bool canBuild);

#endif // BUILD_PREVIEW_RENDER_H
