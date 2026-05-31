#ifndef EHOMELESS_H
#define EHOMELESS_H

#include "ebasicpatroler.h"
#include "engine/ecityid.h"

class GameBoard;
class eTile;

class eHomeless : public eBasicPatroler {
public:
    eHomeless(GameBoard& board);

    static void spawn(GameBoard& board, eTile* tile,
                      eCityId cid, int spawnCount, int waitTime);
};

#endif // EHOMELESS_H
