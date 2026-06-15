#ifndef WATER_TO_BEACH_H
#define WATER_TO_BEACH_H

class eTile;
enum class eWorldDirection;

namespace WaterToBeach {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // WATER_TO_BEACH_H
