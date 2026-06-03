#ifndef TRIBUTE_H
#define TRIBUTE_H

#include "difficulty.h"
#include "eresourcetype.h"

class WorldCity;
class GameBoard;

struct TributePayment {
    eResourceType fType;
    int fCount;
};

namespace TributeHelpers {
    TributePayment receiveTribute(const WorldCity& city);
    TributePayment payTribute(const WorldCity& city,
                               const Difficulty diff);
    void receiveTributeFromCity(GameBoard& board,
                                 const ePlayerId pid,
                                 const stdsptr<WorldCity>& city,
                                 const bool postpone);
    void payTributeToCity(GameBoard& board,
                          const eCityId playerCityId,
                          const stdsptr<WorldCity>& parentCity);
}

#endif // TRIBUTE_H
