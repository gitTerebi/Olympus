#ifndef EPRICECHANGEEVENT_H
#define EPRICECHANGEEVENT_H

#include "eresourcecountcityevent.h"

#include "engine/eresourcetype.h"

class ePriceChangeEvent : public eResourceCountCityEvent {
public:
    ePriceChangeEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
};

#endif // EPRICECHANGEEVENT_H
