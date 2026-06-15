#ifndef EEARTHQUAKEEVENT_H
#define EEARTHQUAKEEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "ecounteventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class SaveArchive;

class eEarthquakeEvent : public eGameEvent,
                         public ePointEventValue,
                         public eCountEventValue,
                         public eGodEventValue,
                         public eGodReasonEventValue {
public:
    eEarthquakeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);

    void trigger() override;
    std::string longName() const override;

protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // EEARTHQUAKEEVENT_H
