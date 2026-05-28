#ifndef WAREHOUSE_BASE_H
#define WAREHOUSE_BASE_H

#include "estoragebuilding.h"

class WarehouseBase : public eStorageBuilding {
public:
    using eStorageBuilding::eStorageBuilding;

    using eXY = std::vector<std::pair<double, double>>;
    void getSpaceOverlay(const eTileSize size,
                         std::vector<eOverlay>& os,
                         const std::pair<double, double>& xy,
                         const int id) const;
    void getSpaceOverlays(const eTileSize size,
                          std::vector<eOverlay>& os,
                          const eXY& xy) const;
};

#endif // WAREHOUSE_BASE_H
