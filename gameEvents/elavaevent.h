#ifndef ELAVAEVENT_H
#define ELAVAEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "egodeventvalue.h"
#include "egodreasoneventvalue.h"

class eLavaEvent : public eGameEvent,
                   public ePointEventValue,
                   public eGodEventValue,
                   public eGodReasonEventValue {
public:
    eLavaEvent(const eCityId cid,
               const eGameEventBranch branch,
               eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void loadResources() const override;
};

#endif // ELAVAEVENT_H
