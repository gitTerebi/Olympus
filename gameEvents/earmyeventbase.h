#ifndef EARMYEVENTBASE_H
#define EARMYEVENTBASE_H

#include "egameevent.h"

#include "characters/eenlistedforces.h"
#include "engine/e-worldcity.h"

class eSaveArchive;

class eArmyEventBase : public eGameEvent {
public:
    eArmyEventBase(const eCityId cid,
                   const eGameEventType type,
                   const eGameEventBranch branch,
                   eGameBoard& board);
    ~eArmyEventBase();

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    const eEnlistedForces& forces() const { return mForces; }
    const stdsptr<eWorldCity>& city() const { return mCity; }
protected:
    void planArmyReturn();
    void planArmyReturn(const int travelTime);

    void removeArmyEvent();
    void serializeJson(eJsonArchive& ar) override {
        eGameEvent::serializeJson(ar);
        mForces.serializeJson(ar, gameBoard());
        ar.cityRef("mCity", mCity, *gameBoard());
    }

private:
    void serialize(eSaveArchive& ar);
protected:

    eEnlistedForces mForces;
    stdsptr<eWorldCity> mCity;
};

#endif // EARMYEVENTBASE_H
