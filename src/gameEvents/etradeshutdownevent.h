#ifndef ETRADESHUTDOWNEVENT_H
#define ETRADESHUTDOWNEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"

class eTradeShutDownEvent : public eGameEvent,
                            public eCityEventValue {
public:
    eTradeShutDownEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // ETRADESHUTDOWNEVENT_H
