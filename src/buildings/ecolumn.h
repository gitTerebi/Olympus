#ifndef ECOLUMN_H
#define ECOLUMN_H

#include "eaestheticsbuilding.h"

class eColumn : public eAestheticsBuilding {
public:
    using eAestheticsBuilding::eAestheticsBuilding;

    std::vector<Overlay> getOverlays(const eTileSize size) const;
};

class eDoricColumn : public eColumn {
public:
    eDoricColumn(GameBoard& board, const eCityId cid);
};

class eIonicColumn : public eColumn {
public:
    eIonicColumn(GameBoard& board, const eCityId cid);
};

class eCorinthianColumn : public eColumn {
public:
    eCorinthianColumn(GameBoard& board, const eCityId cid);
};


#endif // ECOLUMN_H
