#ifndef EHEPHAESTUSSANCTUARY_H
#define EHEPHAESTUSSANCTUARY_H

#include "esanctuary.h"

class eHephaestusSanctuary : public eSanctuary {
public:
    eHephaestusSanctuary(const int sw, const int sh,
                         eGameBoard& board, const eCityId cid);
};

#endif // EHEPHAESTUSSANCTUARY_H
