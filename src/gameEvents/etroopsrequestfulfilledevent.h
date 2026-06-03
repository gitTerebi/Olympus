#ifndef ETROOPSREQUESTFULFILLEDEVENT_H
#define ETROOPSREQUESTFULFILLEDEVENT_H

#include "eplayerconquesteventbase.h"

class eTroopsRequestFulfilledEvent : public ePlayerConquestEventBase {
public:
    eTroopsRequestFulfilledEvent(const eCityId cid,
                                 const eGameEventBranch branch,
                                 GameBoard& board);

    void initialize(const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city,
                    const stdsptr<WorldCity>& rivalCity);

    void trigger() override;
    std::string longName() const override;
private:
    stdsptr<WorldCity> mRivalCity;
};

#endif // ETROOPSREQUESTFULFILLEDEVENT_H
