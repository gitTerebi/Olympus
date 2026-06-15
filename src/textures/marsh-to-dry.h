#ifndef MARSH_TO_DRY_H
#define MARSH_TO_DRY_H

class eTile;
enum class eWorldDirection;

namespace MarshToDry {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // MARSH_TO_DRY_H
