#ifndef EWHEATFARM_H
#define EWHEATFARM_H

#include "efarmbase.h"

class eWheatFarm : public eFarmBase {
public:
    eWheatFarm(eGameBoard& board, const eCityId cid);
};

#endif // EWHEATFARM_H
