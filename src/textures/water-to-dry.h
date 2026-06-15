#ifndef WATER_TO_DRY_H
#define WATER_TO_DRY_H

class eTile;
enum class eWorldDirection;

namespace WaterToDry {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // WATER_TO_DRY_H
