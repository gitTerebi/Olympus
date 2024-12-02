#ifndef EONIONFARM_H
#define EONIONFARM_H

#include "efarmbase.h"

class eOnionFarm : public eFarmBase {
public:
    eOnionFarm(eGameBoard& board, const eCityId cid);
};

#endif // EONIONFARM_H
