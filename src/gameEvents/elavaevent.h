#ifndef ELAVAEVENT_H
#define ELAVAEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "gods/egodeventvalue.h"
#include "gods/egodreasoneventvalue.h"

class SaveArchive;

class eLavaEvent : public eGameEvent,
                   public ePointEventValue,
                   public eGodEventValue,
                   public eGodReasonEventValue {
public:
    eLavaEvent(const eCityId cid,
               const eGameEventBranch branch,
               GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void loadResources() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // ELAVAEVENT_H
