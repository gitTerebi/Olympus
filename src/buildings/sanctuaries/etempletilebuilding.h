#ifndef ETEMPLETILEBUILDING_H
#define ETEMPLETILEBUILDING_H

#include "esanctbuilding.h"

class eTempleTileBuilding : public eSanctBuilding {
public:
    eTempleTileBuilding(const int id, GameBoard& board,
                        const eCityId cid);

    std::shared_ptr<Texture>
        getTexture(const eTileSize size) const;
    std::vector<Overlay>
        getOverlays(const eTileSize size) const;

    std::shared_ptr<Texture>
        getTileTexture(const eTileSize size) const;

    int id() const { return mId; }
private:
    const int mId;
};

#endif // ETEMPLETILEBUILDING_H
