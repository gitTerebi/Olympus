#include "water-to-dry.h"

#include "water-to-x.h"

int WaterToDry::get(eTile* const tile,
                     const eWorldDirection dir) {
    return WaterToX::get(tile, eTerrain::dryBased | eTerrain::beach, dir);
}
