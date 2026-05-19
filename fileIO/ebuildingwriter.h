#ifndef EBUILDINGWRITER_H
#define EBUILDINGWRITER_H

#include "buildings/ebuilding.h"
#include "fileIO/ewritestream.h"

class eSaveArchive;

namespace eBuildingWriter {
    void sWrite(const eBuilding* const b,
                eSaveArchive& ar);
};

#endif // EBUILDINGWRITER_H
