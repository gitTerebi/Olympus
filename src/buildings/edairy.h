#ifndef EDAIRY_H
#define EDAIRY_H

#include "eshepherbuildingbase.h"

class Goatherd;

class eDairy : public eShepherBuildingBase {
public:
    eDairy(GameBoard& board, const eCityId cid);

    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    const std::vector<BuildingTextures>& mTextures;
};

#endif // EDAIRY_H
