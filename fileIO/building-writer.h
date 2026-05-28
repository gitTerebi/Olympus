#ifndef BUILDING_WRITER_H
#define BUILDING_WRITER_H

#include "buildings/ebuilding.h"
#include "fileIO/ewritestream.h"

class eSaveArchive;

namespace BuildingArchive {
    void save(const eBuilding* const b,
              eSaveArchive& ar);
};

#endif // BUILDING_WRITER_H
