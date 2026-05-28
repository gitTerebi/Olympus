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
                   GameBoard& board);
    ~eArmyEventBase();

    const eEnlistedForces& forces() const { return mForces; }
    const stdsptr<eWorldCity>& city() const { return mCity; }
protected:
    void planArmyReturn();
    void planArmyReturn(const int travelTime);

    void removeArmyEvent();

    void serializeFields(eSaveArchive& ar) override;

    eEnlistedForces mForces;
    stdsptr<eWorldCity> mCity;
};

#endif // EARMYEVENTBASE_H
