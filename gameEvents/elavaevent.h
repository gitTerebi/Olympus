#ifndef ELAVAEVENT_H
#define ELAVAEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class eSaveArchive;

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
private:
    void serialize(eSaveArchive& ar);
};

#endif // ELAVAEVENT_H
