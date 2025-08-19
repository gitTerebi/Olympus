#ifndef ETRADESHUTDOWNEVENT_H
#define ETRADESHUTDOWNEVENT_H

#include "egameevent.h"
#include "ecityevent.h"

class eTradeShutDownEvent : public eGameEvent,
                            public eCityEvent {
public:
    eTradeShutDownEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // ETRADESHUTDOWNEVENT_H
