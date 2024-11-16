#ifndef ECOLUMN_H
#define ECOLUMN_H

#include "eaestheticsbuilding.h"

class eColumn : public eAestheticsBuilding {
public:
    using eAestheticsBuilding::eAestheticsBuilding;

    std::vector<eOverlay> getOverlays(const eTileSize size) const;
};

class eDoricColumn : public eColumn {
public:
    eDoricColumn(eGameBoard& board, const eCityId cid);
};

class eIonicColumn : public eColumn {
public:
    eIonicColumn(eGameBoard& board, const eCityId cid);
};

class eCorinthianColumn : public eColumn {
public:
    eCorinthianColumn(eGameBoard& board, const eCityId cid);
};


#endif // ECOLUMN_H
