#ifndef EREINFORCEMENTSEVENT_H
#define EREINFORCEMENTSEVENT_H

#include "earmyeventbase.h"

class eReinforcementsEvent : public eArmyEventBase {
public:
    eReinforcementsEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<eWorldCity>& city);

    void trigger() override;

    std::string longName() const override
    { return "Reinforcements"; }
};

#endif // EREINFORCEMENTSEVENT_H
