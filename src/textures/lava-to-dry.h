#ifndef LAVA_TO_DRY_H
#define LAVA_TO_DRY_H

class eTile;
enum class eWorldDirection;

namespace LavaToDry {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // LAVA_TO_DRY_H
