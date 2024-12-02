#ifndef EPODIUM_H
#define EPODIUM_H

#include "epatroltarget.h"

class ePodium : public ePatrolTarget {
public:
    ePodium(eGameBoard& board, const eCityId cid);
};

#endif // EPODIUM_H
