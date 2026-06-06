#ifndef EARMYRETURNEVENT_H
#define EARMYRETURNEVENT_H

#include "army-event-base.h"

class ArmyReturnEvent : public ArmyEventBase {
public:
    ArmyReturnEvent(const eCityId cid,
                    const eGameEventBranch branch,
                    GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city);

    void trigger() override;
    std::string longName() const override;
};


#endif // EARMYRETURNEVENT_H
