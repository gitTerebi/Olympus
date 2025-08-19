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

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setDuration(const int d) { mDuration = d; }
    bool duration() const { return mDuration; }
private:
    bool mEnd = false;
    int mDuration = 12;
    int mTroopsTaken = 0;
};

#endif // ERIVALARMYAWAYEVENT_H
