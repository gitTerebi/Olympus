#ifndef FERTILE_TO_DRY_H
#define FERTILE_TO_DRY_H

class eTile;
enum class eWorldDirection;

enum class FertileToDryId {
    somewhere,
    none
};

namespace FertileToDry {
    FertileToDryId get(eTile* const tile, const eWorldDirection dir);
};

#endif // FERTILE_TO_DRY_H
