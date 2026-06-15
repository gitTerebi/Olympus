#include "water-to-beach.h"

#include "water-to-x.h"

int WaterToBeach::get(eTile* const tile,
                       const eWorldDirection dir) {
    return WaterToX::get(tile, eTerrain::beach, dir);
}
