#ifndef ETRADEOPENUPEVENT_H
#define ETRADEOPENUPEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"

class eTradeOpenUpEvent : public eGameEvent,
                          public eCityEventValue {
public:
    eTradeOpenUpEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
};

#endif // ETRADEOPENUPEVENT_H
