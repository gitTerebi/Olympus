#ifndef EWINEVENDOR_H
#define EWINEVENDOR_H

#include "evendor.h"

class eWineVendor : public eVendor {
public:
    eWineVendor(eGameBoard& board, const eCityId cid);
};

#endif // EWINEVENDOR_H
