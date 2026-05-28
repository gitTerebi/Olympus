#ifndef ELABORATORY_H
#define ELABORATORY_H

#include "epatroltarget.h"

class eLaboratory : public ePatrolTarget {
public:
    eLaboratory(GameBoard& board, const eCityId cid);
};

#endif // ELABORATORY_H
