#ifndef FOREST_TO_DRY_H
#define FOREST_TO_DRY_H

class eTile;
enum class eWorldDirection;

enum class ForestToDryId {
    somewhere,
    none
};

namespace ForestToDry {
    ForestToDryId get(eTile* const tile, const eWorldDirection dir);
};

#endif // FOREST_TO_DRY_H
