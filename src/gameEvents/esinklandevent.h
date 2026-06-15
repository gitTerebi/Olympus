#ifndef ESINKLANDEVENT_H
#define ESINKLANDEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class SaveArchive;

class eSinkLandEvent : public eGameEvent,
                       public eCountEventValue,
                       public eGodEventValue,
                       public eGodReasonEventValue {
public:
    eSinkLandEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void loadResources() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // ESINKLANDEVENT_H
