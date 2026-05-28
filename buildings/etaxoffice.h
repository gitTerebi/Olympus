#ifndef ETAXOFFICE_H
#define ETAXOFFICE_H

#include "epatrolbuilding.h"

class eTaxOffice : public ePatrolBuilding {
public:
    eTaxOffice(GameBoard& board, const eCityId cid);
};

#endif // ETAXOFFICE_H
