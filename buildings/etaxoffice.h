#ifndef ETAXOFFICE_H
#define ETAXOFFICE_H

#include "epatrolbuilding.h"

class eTaxOffice : public ePatrolBuilding {
public:
    eTaxOffice(eGameBoard& board, const eCityId cid);
};

#endif // ETAXOFFICE_H
