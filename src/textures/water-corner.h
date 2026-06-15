#ifndef WATER_CORNER_H
#define WATER_CORNER_H

class eTile;
enum class eWorldDirection;

namespace WaterCorner {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // WATER_CORNER_H
