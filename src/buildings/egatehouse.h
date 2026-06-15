#ifndef EGATEHOUSE_H
#define EGATEHOUSE_H

#include "ebuilding.h"

class eGatehouse : public eBuilding {
public:
    eGatehouse(GameBoard& board, const bool r,
               const eCityId cid);

    void erase() override;

    std::shared_ptr<Texture>
    getTexture(const eTileSize) const override { return nullptr; }

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    std::shared_ptr<Texture>
    getTextureSides(const eTileSize size) const;
    Overlay
    getTextureEntrance(const eTileSize size) const;
    Overlay
    getTextureEntranceBack(const eTileSize size) const;
    Overlay
    getTextureEntranceOverlay(const eTileSize size) const;

    bool rotated() const { return mRotated; }
private:
    const bool mRotated;
};


#endif // EGATEHOUSE_H
