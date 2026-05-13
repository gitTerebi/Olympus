#ifndef ECITYBECOMESEVENT_H
#define ECITYBECOMESEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"
#include "eattackingcityeventvalue.h"
#include "fileIO/ejsonarchive.h"

enum class eCityBecomesType {
    ally, rival, vassal,
    active, inactive,
    visible, invisible,
    rebellionOver, conquered
};

class eSaveArchive;

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
    void serializeJson(eJsonArchive& ar) override {
        eGameEvent::serializeJson(ar);
        eCityEventValue::serializeJson(ar, *gameBoard());
        eAttackingCityEventValue::serializeJson(ar, *gameBoard());
        ar.field("mType", mType);
    }

private:
    void serialize(eSaveArchive& ar);

    eCityBecomesType mType = eCityBecomesType::ally;
};

#endif // ECITYBECOMESEVENT_H
