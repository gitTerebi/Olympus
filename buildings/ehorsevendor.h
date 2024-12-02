#ifndef EHORSEVENDOR_H
#define EHORSEVENDOR_H

#include "evendor.h"

class eHorseVendor : public eVendor {
public:
    eHorseVendor(eGameBoard& board, const eCityId cid);
};

#endif // EHORSEVENDOR_H
