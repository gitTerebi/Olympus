#ifndef EARMYRETURNEVENT_H
#define EARMYRETURNEVENT_H

#include "earmyeventbase.h"

class eArmyReturnEvent : public eArmyEventBase {
public:
    eArmyReturnEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city);

    void trigger() override;
    std::string longName() const override;
};


#endif // EARMYRETURNEVENT_H
