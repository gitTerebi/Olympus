#ifndef EECONOMICMILITARYCHANGEEVENTBASE_H
#define EECONOMICMILITARYCHANGEEVENTBASE_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "ecityeventvalue.h"

class eSaveArchive;

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
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);
};

#endif // EECONOMICMILITARYCHANGEEVENTBASE_H
