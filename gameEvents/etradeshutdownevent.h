#ifndef ETRADESHUTDOWNEVENT_H
#define ETRADESHUTDOWNEVENT_H

#include "ebasiccityevent.h"

class eTradeShutDownEvent : public eBasicCityEvent {
public:
    eTradeShutDownEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;;
};

#endif // ETRADESHUTDOWNEVENT_H
