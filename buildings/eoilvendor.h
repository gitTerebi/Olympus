#ifndef EOILVENDOR_H
#define EOILVENDOR_H

#include "evendor.h"

class eOilVendor : public eVendor {
public:
    eOilVendor(GameBoard& board, const eCityId cid);
};

#endif // EOILVENDOR_H
