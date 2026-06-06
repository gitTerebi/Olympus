#ifndef EARMYEVENTBASE_H
#define EARMYEVENTBASE_H

#include "../egameevent.h"

#include "characters/eenlistedforces.h"
#include "engine/world-city.h"

class eSaveArchive;

class ArmyEventBase : public eGameEvent {
public:
    ArmyEventBase(const eCityId cid,
                  const eGameEventType type,
                  const eGameEventBranch branch,
                  GameBoard& board);
    ~ArmyEventBase();

    const eEnlistedForces& forces() const { return mForces; }
    const stdsptr<WorldCity>& city() const { return mCity; }
protected:
    void planArmyReturn();
    void planArmyReturn(const int travelTime);

    void removeArmyEvent();

    void serializeFields(eSaveArchive& ar) override;

    eEnlistedForces mForces;
    stdsptr<WorldCity> mCity;
};

#endif // EARMYEVENTBASE_H
