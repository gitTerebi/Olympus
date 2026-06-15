#ifndef WATER_TO_X_H
#define WATER_TO_X_H

#include "engine/etile.h"

namespace WaterToX {
    int get(eTile* const tile, const eTerrain x,
            const eWorldDirection dir);
};

#endif // WATER_TO_X_H
