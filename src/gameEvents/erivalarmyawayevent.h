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
                        GameBoard& board);

    void trigger() override;
    std::string longName() const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
};

#endif // ERIVALARMYAWAYEVENT_H
