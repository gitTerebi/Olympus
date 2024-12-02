#ifndef EFLEECEVENDOR_H
#define EFLEECEVENDOR_H

#include "evendor.h"

class eFleeceVendor : public eVendor {
public:
    eFleeceVendor(eGameBoard& board, const eCityId cid);
};

#endif // EFLEECEVENDOR_H
