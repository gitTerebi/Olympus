#ifndef ETRADEOPENUPEVENT_H
#define ETRADEOPENUPEVENT_H

#include "ebasiccityevent.h"

class eTradeOpenUpEvent : public eBasicCityEvent {
public:
    eTradeOpenUpEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;;
};

#endif // ETRADEOPENUPEVENT_H
