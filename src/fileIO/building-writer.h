#ifndef BUILDING_WRITER_H
#define BUILDING_WRITER_H

#include "buildings/ebuilding.h"
#include "fileIO/write-stream.h"

class SaveArchive;

namespace BuildingArchive {
    void save(const eBuilding* const b,
              SaveArchive& ar);
};

#endif // BUILDING_WRITER_H
