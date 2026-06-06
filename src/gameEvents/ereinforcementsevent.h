#ifndef EREINFORCEMENTSEVENT_H
#define EREINFORCEMENTSEVENT_H

#include "conquest/army-event-base.h"

class eReinforcementsEvent : public ArmyEventBase {
public:
    eReinforcementsEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city);

    void trigger() override;

    std::string longName() const override
    { return "Reinforcements"; }
};

#endif // EREINFORCEMENTSEVENT_H
