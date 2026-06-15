#ifndef EGRANARY_H
#define EGRANARY_H

#include "estoragebuilding.h"

#include "textures/building-textures.h"

class eGranary : public eStorageBuilding {
public:
    eGranary(GameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
    std::vector<eOverlay> getOverlays(const eTileSize size) const;
private:
    const std::vector<BuildingTextures>& mTextures;
};

#endif // EGRANARY_H
