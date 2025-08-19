#ifndef ECITYBECOMESEVENT_H
#define ECITYBECOMESEVENT_H

#include "egameevent.h"
#include "ecityevent.h"

enum class eCityBecomesType {
    ally, rival, vassal,
    active, inactive,
    visible, invisible,
    conquered
};

class eCityBecomesEvent : public eGameEvent,
                          public eCityEvent {
public:
    eCityBecomesEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void setCity(const stdsptr<eWorldCity>& c) { mCity = c; }
    const stdsptr<eWorldCity>& city() const { return mCity; }

    void setConqueringCity(const stdsptr<eWorldCity>& c) { mConqueringCity = c; }
    const stdsptr<eWorldCity>& conqueringCity() const { return mConqueringCity; }

    void setType(const eCityBecomesType t) { mType = t; }
    eCityBecomesType type() const { return mType; }
private:
    eCityBecomesType mType = eCityBecomesType::ally;
    stdsptr<eWorldCity> mCity;
    stdsptr<eWorldCity> mConqueringCity;
};

#endif // ECITYBECOMESEVENT_H
