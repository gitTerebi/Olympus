#ifndef ECARROTFARM_H
#define ECARROTFARM_H

#include "efarmbase.h"

class eCarrotFarm : public eFarmBase {
public:
    eCarrotFarm(eGameBoard& board, const eCityId cid);
};

#endif // ECARROTFARM_H
