#ifndef EBUILDINGREADER_H
#define EBUILDINGREADER_H

#include "buildings/ebuilding.h"
#include "fileIO/ereadstream.h"
#include "fileIO/ejsonarchive.h"

namespace eBuildingReader {
    stdsptr<eBuilding> sRead(
            eGameBoard& board, const eBuildingType type,
            eReadStream& src);
    [[nodiscard]] stdsptr<eBuilding> sReadJson(
            eGameBoard& board, const eBuildingType type,
            eJsonArchive& ar);
};

#endif // EBUILDINGREADER_H
