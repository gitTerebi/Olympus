#ifndef ECITYBECOMESEVENT_H
#define ECITYBECOMESEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"
#include "eattackingcityeventvalue.h"

enum class eCityBecomesType {
    ally, rival, vassal,
    active, inactive,
    visible, invisible,
    rebellionOver, conquered
};

class eCityBecomesEvent : public eGameEvent,
                          public eCityEventValue,
                          public eAttackingCityEventValue {
public:
    eCityBecomesEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void setType(const eCityBecomesType t) { mType = t; }
    eCityBecomesType type() const { return mType; }
private:
    eCityBecomesType mType = eCityBecomesType::ally;
};

#endif // ECITYBECOMESEVENT_H
