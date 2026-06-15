#ifndef BUILDING_READER_H
#define BUILDING_READER_H

#include "buildings/ebuilding.h"
#include "fileIO/read-stream.h"

class SaveArchive;

namespace BuildingArchive {
    stdsptr<eBuilding> load(
            GameBoard& board, const eBuildingType type,
            SaveArchive& ar);
};

#endif // BUILDING_READER_H
