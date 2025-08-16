#ifndef ESUPPLYDEMANDCHANGEEVENT_H
#define ESUPPLYDEMANDCHANGEEVENT_H

#include "eresourcecityevent.h"

#include "engine/eresourcetype.h"

class eSupplyDemandChangeEvent : public eResourceCityEvent {
public:
    using eResourceCityEvent::eResourceCityEvent;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    int by() const { return mBy; }
    void setBy(const int by) { mBy = by; }
private:
    int mBy = 0;
};

#endif // ESUPPLYDEMANDCHANGEEVENT_H
