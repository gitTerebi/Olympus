#ifndef EPARK_H
#define EPARK_H

#include "ebuilding.h"
#include "textures/building-textures.h"

class ePark : public eBuilding {
public:
    ePark(GameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    const std::vector<BuildingTextures>& mTextures;
};

#endif // EPARK_H
