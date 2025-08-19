#ifndef EWAGECHANGEEVENT_H
#define EWAGECHANGEEVENT_H

#include "egameevent.h"
#include "ecountevent.h"

class eWageChangeEvent : public eGameEvent,
                         public eCountEvent {
public:
    eWageChangeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     eGameBoard& board);

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void trigger() override;
    std::string longName() const override;
};

#endif // EWAGECHANGEEVENT_H
