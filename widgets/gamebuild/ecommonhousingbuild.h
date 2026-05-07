#ifndef ECOMMONHOUSINGBUILD_H
#define ECOMMONHOUSINGBUILD_H

#include "engine/ecityid.h"

#include <vector>

class eGameBoard;

struct eCommonHousingTileRect {
    int fX;
    int fY;
    int fW;
    int fH;
};

std::vector<eCommonHousingTileRect> commonHousingBuildRects(
        eGameBoard* board, eCityId cid, ePlayerId pid, bool editorMode,
        int pressedTX, int pressedTY, int hoverTX, int hoverTY);

eCommonHousingTileRect commonHousingBuildBounds(
        const std::vector<eCommonHousingTileRect>& rects);

bool buildCommonHousing(eGameBoard* board, eCityId cid, ePlayerId pid, bool editorMode,
                        int pressedTX, int pressedTY, int hoverTX, int hoverTY, eCityId viewedCityId);

#endif // ECOMMONHOUSINGBUILD_H
