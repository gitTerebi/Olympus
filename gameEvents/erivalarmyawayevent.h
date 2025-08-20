#ifndef ERIVALARMYAWAYEVENT_H
#define ERIVALARMYAWAYEVENT_H

#include "egameevent.h"
#include "ecityevent.h"

class eRivalArmyAwayEvent : public eGameEvent,
                            public eCityEvent {
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
