#ifndef EARMSVENDOR_H
#define EARMSVENDOR_H

#include "evendor.h"

class eArmsVendor : public eVendor {
public:
    eArmsVendor(GameBoard& board, const eCityId cid);
};

#endif // EARMSVENDOR_H
