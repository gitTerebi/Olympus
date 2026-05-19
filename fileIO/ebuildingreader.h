#ifndef EBUILDINGREADER_H
#define EBUILDINGREADER_H

#include "buildings/ebuilding.h"
#include "fileIO/ereadstream.h"

class eSaveArchive;

namespace eBuildingReader {
    stdsptr<eBuilding> sRead(
            eGameBoard& board, const eBuildingType type,
            eSaveArchive& ar);
};

#endif // EBUILDINGREADER_H
