#ifndef ECARDINGSHED_H
#define ECARDINGSHED_H

#include "eshepherbuildingbase.h"

class Shepherd;

class eCardingShed : public eShepherBuildingBase {
public:
    eCardingShed(GameBoard& board, const eCityId cid);

    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    const std::vector<BuildingTextures>& mTextures;
};

#endif // ECARDINGSHED_H
