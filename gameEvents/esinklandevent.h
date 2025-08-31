#ifndef ESINKLANDEVENT_H
#define ESINKLANDEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "egodeventvalue.h"
#include "egodreasoneventvalue.h"

class eSinkLandEvent : public eGameEvent,
                       public eCountEventValue,
                       public eGodEventValue,
                       public eGodReasonEventValue {
public:
    eSinkLandEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void loadResources() const override;
};

#endif // ESINKLANDEVENT_H
