#ifndef ESUPPLYDEMANDCHANGEEVENT_H
#define ESUPPLYDEMANDCHANGEEVENT_H

#include "egameevent.h"
#include "eresourceeventvalue.h"
#include "ecounteventvalue.h"
#include "ecityeventvalue.h"

class SaveArchive;

class eSupplyDemandChangeEvent : public eGameEvent,
                                 public eResourceEventValue,
                                 public eCountEventValue,
                                 public eCityEventValue {
public:
    eSupplyDemandChangeEvent(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        GameBoard& board);
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // ESUPPLYDEMANDCHANGEEVENT_H
