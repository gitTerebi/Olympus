#ifndef EEARTHQUAKEEVENT_H
#define EEARTHQUAKEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "ecounteventvalue.h"
#include "egodeventvalue.h"
#include "egodreasoneventvalue.h"

class eSaveArchive;

class eEarthquakeEvent : public eGameEvent,
                         public ePointEventValue,
                         public eCountEventValue,
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
private:
    void serialize(eSaveArchive& ar);
};

#endif // EEARTHQUAKEEVENT_H
