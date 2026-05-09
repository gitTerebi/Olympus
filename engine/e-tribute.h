#ifndef ETRIBUTE_H
#define ETRIBUTE_H

#include "edifficulty.h"
#include "eresourcetype.h"

class eWorldCity;
class eGameBoard;

struct eTributePayment {
    eResourceType fType;
    int fCount;
};

namespace eTributeHelpers {
    eTributePayment receiveTribute(const eWorldCity& city);
    eTributePayment payTribute(const eWorldCity& city,
                               const eDifficulty diff);
    void receiveTributeFromCity(eGameBoard& board,
                                 const ePlayerId pid,
                                 const stdsptr<eWorldCity>& city,
                                 const bool postpone);
    void payTributeToCity(eGameBoard& board,
                          const eCityId playerCityId,
                          const stdsptr<eWorldCity>& parentCity);
}

#endif // ETRIBUTE_H
