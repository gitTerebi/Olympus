#ifndef EOBSERVATORY_H
#define EOBSERVATORY_H

#include "epatroltarget.h"

class eObservatory : public ePatrolTarget {
public:
    eObservatory(GameBoard& board, const eCityId cid);
};

#endif // EOBSERVATORY_H
