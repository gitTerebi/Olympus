#ifndef BUILDING_READER_H
#define BUILDING_READER_H

#include "buildings/ebuilding.h"
#include "fileIO/ereadstream.h"

class eSaveArchive;

namespace BuildingArchive {
    stdsptr<eBuilding> load(
            GameBoard& board, const eBuildingType type,
            eSaveArchive& ar);
};

#endif // BUILDING_READER_H
