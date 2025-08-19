#ifndef ETRADEOPENUPEVENT_H
#define ETRADEOPENUPEVENT_H

#include "egameevent.h"
#include "ecityevent.h"

class eTradeOpenUpEvent : public eGameEvent,
                          public eCityEvent {
public:
    eTradeOpenUpEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // ETRADEOPENUPEVENT_H
