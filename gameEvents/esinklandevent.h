#ifndef ESINKLANDEVENT_H
#define ESINKLANDEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class eSaveArchive;

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
    void serializeJson(eJsonArchive& ar) override;

    void loadResources() const override;
private:
    void serialize(eSaveArchive& ar);
};

#endif // ESINKLANDEVENT_H
