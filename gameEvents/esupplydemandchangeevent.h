#ifndef ESUPPLYDEMANDCHANGEEVENT_H
#define ESUPPLYDEMANDCHANGEEVENT_H

#include "egameevent.h"
#include "eresourceeventvalue.h"
#include "ecounteventvalue.h"
#include "ecityeventvalue.h"

class eSupplyDemandChangeEvent : public eGameEvent,
                                 public eResourceEventValue,
                                 public eCountEventValue,
                                 public eCityEventValue {
public:
    using eGameEvent::eGameEvent;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // ESUPPLYDEMANDCHANGEEVENT_H
