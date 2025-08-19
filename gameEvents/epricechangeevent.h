#ifndef EPRICECHANGEEVENT_H
#define EPRICECHANGEEVENT_H

#include "egameevent.h"
#include "eresourceevent.h"
#include "ecountevent.h"

class ePriceChangeEvent : public eGameEvent,
                          public eResourceEvent,
                          public eCountEvent {
public:
    ePriceChangeEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // EPRICECHANGEEVENT_H
