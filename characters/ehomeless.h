#ifndef EHOMELESS_H
#define EHOMELESS_H

#include "ebasicpatroler.h"
#include "engine/ecityid.h"

class eGameBoard;
class eTile;

class eHomeless : public eBasicPatroler {
public:
    eHomeless(eGameBoard& board);

    static void spawn(eGameBoard& board, eTile* tile,
                      eCityId cid, int spawnCount, int waitTime);
};

#endif // EHOMELESS_H
