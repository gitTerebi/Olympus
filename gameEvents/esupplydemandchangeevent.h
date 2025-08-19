#ifndef ESUPPLYDEMANDCHANGEEVENT_H
#define ESUPPLYDEMANDCHANGEEVENT_H

#include "egameevent.h"
#include "eresourceevent.h"
#include "ecityevent.h"

class eSupplyDemandChangeEvent : public eGameEvent,
                                 public eResourceEvent,
                                 public eCityEvent {
public:
    using eGameEvent::eGameEvent;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    int by() const { return mBy; }
    void setBy(const int by) { mBy = by; }
private:
    int mBy = 0;
};

#endif // ESUPPLYDEMANDCHANGEEVENT_H
