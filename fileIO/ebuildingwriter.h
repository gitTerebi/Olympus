#ifndef EBUILDINGWRITER_H
#define EBUILDINGWRITER_H

#include "buildings/ebuilding.h"
#include "fileIO/ewritestream.h"
#include "fileIO/ejsonarchive.h"

namespace eBuildingWriter {
    void sWrite(const eBuilding* const b,
                eWriteStream& dst);
    void sWriteJson(const eBuilding* b, eJsonArchive& ar);
};

#endif // EBUILDINGWRITER_H
