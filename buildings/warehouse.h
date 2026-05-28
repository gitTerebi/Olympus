#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "warehouse-base.h"

class Warehouse : public WarehouseBase {
public:
    Warehouse(eGameBoard& board, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
    eTextureSpace getTextureSpace(const int tx, const int ty,
                                  const eTileSize size) const;
    std::vector<eOverlay> getOverlays(const eTileSize size) const;
};

#endif // WAREHOUSE_H
