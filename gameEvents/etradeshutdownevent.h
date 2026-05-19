#ifndef ETRADESHUTDOWNEVENT_H
#define ETRADESHUTDOWNEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"

class eTradeShutDownEvent : public eGameEvent,
                            public eCityEventValue {
public:
    eTradeShutDownEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
};

#endif // ETRADESHUTDOWNEVENT_H
