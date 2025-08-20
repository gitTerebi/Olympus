#ifndef ERIVALARMYAWAYEVENT_H
#define ERIVALARMYAWAYEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"

class eRivalArmyAwayEvent : public eGameEvent,
                            public eCityEventValue {
public:
    eRivalArmyAwayEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
};

#endif // ERIVALARMYAWAYEVENT_H
