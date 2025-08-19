#ifndef ESUPPLYDEMANDCHANGEEVENT_H
#define ESUPPLYDEMANDCHANGEEVENT_H

#include "egameevent.h"
#include "eresourceevent.h"
#include "ecountevent.h"
#include "ecityevent.h"

class eSupplyDemandChangeEvent : public eGameEvent,
                                 public eResourceEvent,
                                 public eCountEvent,
                                 public eCityEvent {
public:
    using eGameEvent::eGameEvent;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // ESUPPLYDEMANDCHANGEEVENT_H
