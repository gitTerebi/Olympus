#ifndef EPLAYERCONQUESTEVENT_H
#define EPLAYERCONQUESTEVENT_H

#include "eplayerconquesteventbase.h"

class eInvasionEvent;

class ePlayerConquestEvent : public ePlayerConquestEventBase {
public:
    ePlayerConquestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);

    void initialize(const eDate& date,
                    const eEnlistedForces& forces,
                    const stdsptr<eWorldCity>& city);

    void trigger() override;
    std::string longName() const override;

    bool finished() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    using ePlayerConquestEventBase::planArmyReturn;
private:
    stdptr<eInvasionEvent> mInvasionEvent;
};

#endif // EPLAYERCONQUESTEVENT_H
