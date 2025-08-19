#ifndef EEARTHQUAKEEVENT_H
#define EEARTHQUAKEEVENT_H

#include "egameevent.h"
#include "epointeventbase.h"
#include "ecountevent.h"
#include "egodeventvalue.h"
#include "egodreasoneventvalue.h"

class eEarthquakeEvent : public eGameEvent,
                         public ePointEventBase,
                         public eCountEvent,
                         public eGodEventValue,
                         public eGodReasonEventValue {
public:
    eEarthquakeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // EEARTHQUAKEEVENT_H
