#ifndef ERIVALARMYAWAYEVENT_H
#define ERIVALARMYAWAYEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"

class eSaveArchive;

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
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);
};

#endif // ERIVALARMYAWAYEVENT_H
