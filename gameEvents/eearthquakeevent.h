#ifndef EEARTHQUAKEEVENT_H
#define EEARTHQUAKEEVENT_H

#include "egameevent.h"
#include "epointeventbase.h"

#include "characters/gods/egod.h"

class eEarthquakeEvent : public eGameEvent,
                         public ePointEventBase {
public:
    eEarthquakeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void setGod(const eGodType god) { mGod = god; }
    eGodType god() const { return mGod; }

    void setSize(const int s) { mSize = s; }
    int size() const { return mSize; }
private:
    eGodType mGod = eGodType::zeus;
    int mSize = 100;
};

#endif // EEARTHQUAKEEVENT_H
