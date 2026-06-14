#ifndef EHEATGETTERS_H
#define EHEATGETTERS_H

#include "buildings/ebuilding.h"
#include "engine/boardData/eheatmap.h"

class eTileBase;

enum class GodType;
enum class Difficulty;

namespace eHeatGetters {
    using eHeatGetter = std::function<eHeat(eBuildingType)>;

    eHeat appeal(const eBuildingType type);
    eHeat appeal(const eBuildingType type, Difficulty diff);
    eHeat housing(const eBuildingType type);
    eHeat culture(const eBuildingType type);
    eHeat industry(const eBuildingType type);
    eHeat farming(const eBuildingType type);
    eHeat storage(const eBuildingType type);

    eHeatGetter godLeaning(const GodType gt);

    eHeat empty(const eBuildingType type);
    eHeat any(const eBuildingType type);

    eHeat fertile(eTileBase* const tile);
    eHeat notFertile(eTileBase* const tile);
}

#endif // EHEATGETTERS_H
