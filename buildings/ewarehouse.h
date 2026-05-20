#ifndef EWAREHOUSE_H
#define EWAREHOUSE_H

#include "ewarehousebase.h"

class eWarehouse : public eWarehouseBase {
public:
    eWarehouse(eGameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
    eTextureSpace getTextureSpace(const int tx, const int ty,
                                  const eTileSize size) const;
    std::vector<eOverlay> getOverlays(const eTileSize size) const;
};

#endif // EWAREHOUSE_H
