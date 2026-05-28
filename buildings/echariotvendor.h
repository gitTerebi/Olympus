#ifndef ECHARIOTVENDOR_H
#define ECHARIOTVENDOR_H

#include "evendor.h"

class eChariotVendor : public eVendor {
public:
    eChariotVendor(GameBoard& board, const eCityId cid);
};

#endif // ECHARIOTVENDOR_H
