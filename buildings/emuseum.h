#ifndef EMUSEUM_H
#define EMUSEUM_H

#include "epatroltarget.h"

class eMuseum : public ePatrolTarget {
public:
    eMuseum(eGameBoard& board, const eCityId cid);
    ~eMuseum();
};

#endif // EMUSEUM_H
