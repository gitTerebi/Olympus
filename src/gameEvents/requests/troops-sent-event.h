#ifndef TROOPS_SENT_EVENT_H
#define TROOPS_SENT_EVENT_H

#include "../eplayerconquesteventbase.h"

class TroopsSentEvent : public ePlayerConquestEventBase {
public:
    TroopsSentEvent(const eCityId cid,
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

#endif // TROOPS_SENT_EVENT_H
