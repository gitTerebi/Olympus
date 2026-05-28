#ifndef EDAIRY_H
#define EDAIRY_H

#include "eshepherbuildingbase.h"

class eGoatherd;

class eDairy : public eShepherBuildingBase {
public:
    eDairy(GameBoard& board, const eCityId cid);

    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    const std::vector<eBuildingTextures>& mTextures;
};

#endif // EDAIRY_H
