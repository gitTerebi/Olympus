#ifndef EBIBLIOTHEKE_H
#define EBIBLIOTHEKE_H

#include "epatrolbuilding.h"

class eBibliotheke : public ePatrolBuilding {
public:
    eBibliotheke(GameBoard& board, const eCityId cid);
};

#endif // EBIBLIOTHEKE_H
