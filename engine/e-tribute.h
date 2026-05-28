#ifndef ETRIBUTE_H
#define ETRIBUTE_H

#include "edifficulty.h"
#include "eresourcetype.h"

class eWorldCity;
class GameBoard;

struct eTributePayment {
    eResourceType fType;
    int fCount;
};

namespace eTributeHelpers {
    eTributePayment receiveTribute(const eWorldCity& city);
    eTributePayment payTribute(const eWorldCity& city,
                               const eDifficulty diff);
    void receiveTributeFromCity(GameBoard& board,
                                 const ePlayerId pid,
                                 const stdsptr<eWorldCity>& city,
                                 const bool postpone);
    void payTributeToCity(GameBoard& board,
                          const eCityId playerCityId,
                          const stdsptr<eWorldCity>& parentCity);
}

#endif // ETRIBUTE_H
