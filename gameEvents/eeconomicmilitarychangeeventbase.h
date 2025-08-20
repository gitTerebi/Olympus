#ifndef EECONOMICMILITARYCHANGEEVENTBASE_H
#define EECONOMICMILITARYCHANGEEVENTBASE_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "ecityeventvalue.h"

class eEconomicMilitaryChangeEventBase : public eGameEvent,
                                         public eCountEventValue,
                                         public eCityEventValue {
public:
    eEconomicMilitaryChangeEventBase(
        const eCityId cid,
        const eGameEventType type,
        const eGameEventBranch branch,
        eGameBoard& board);

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // EECONOMICMILITARYCHANGEEVENTBASE_H
