#ifndef ETHEATER_H
#define ETHEATER_H

#include "epatroltarget.h"

class eTheater : public ePatrolTarget {
public:
    eTheater(eGameBoard& board, const eCityId cid);
};

#endif // ETHEATER_H
