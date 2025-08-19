#ifndef EECONOMICMILITARYCHANGEEVENTBASE_H
#define EECONOMICMILITARYCHANGEEVENTBASE_H

#include "egameevent.h"
#include "ecountevent.h"
#include "ecityevent.h"

class eEconomicMilitaryChangeEventBase : public eGameEvent,
                                         public eCountEvent,
                                         public eCityEvent {
public:
    using eGameEvent::eGameEvent;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // EECONOMICMILITARYCHANGEEVENTBASE_H
