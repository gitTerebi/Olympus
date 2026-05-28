#ifndef EONIONFARM_H
#define EONIONFARM_H

#include "efarmbase.h"

class eOnionFarm : public eFarmBase {
public:
    eOnionFarm(GameBoard& board, const eCityId cid);
};

#endif // EONIONFARM_H
