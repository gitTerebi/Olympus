#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "warehouse-base.h"

class Warehouse : public WarehouseBase {
public:
    Warehouse(GameBoard& board, const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    eTextureSpace getTextureSpace(const int tx, const int ty,
                                  const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;
    std::vector<Overlay> getOverlaysAtTile(const eTileSize size,
                                           const int tx,
                                           const int ty) const override;
};

#endif // WAREHOUSE_H
