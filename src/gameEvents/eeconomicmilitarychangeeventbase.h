#ifndef EECONOMICMILITARYCHANGEEVENTBASE_H
#define EECONOMICMILITARYCHANGEEVENTBASE_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "ecityeventvalue.h"

class SaveArchive;

class eEconomicMilitaryChangeEventBase : public eGameEvent,
                                         public eCountEventValue,
                                         public eCityEventValue {
public:
    eEconomicMilitaryChangeEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        GameBoard& board);
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // EECONOMICMILITARYCHANGEEVENTBASE_H
