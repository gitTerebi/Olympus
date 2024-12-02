#ifndef EFOODVENDOR_H
#define EFOODVENDOR_H

#include "evendor.h"

class eFoodVendor : public eVendor {
public:
    eFoodVendor(eGameBoard& board, const eCityId cid);
};

#endif // EFOODVENDOR_H
