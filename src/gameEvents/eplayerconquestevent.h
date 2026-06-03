#ifndef EPLAYERCONQUESTEVENT_H
#define EPLAYERCONQUESTEVENT_H

#include "eplayerconquesteventbase.h"

class eInvasionEvent;
class eSaveArchive;

class ePlayerConquestEvent : public ePlayerConquestEventBase {
public:
    ePlayerConquestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         GameBoard& board);

    void initialize(const eDate& date,
                    const eEnlistedForces& forces,
                    const stdsptr<WorldCity>& city);

    void trigger() override;
    std::string longName() const override;

    bool finished() const override;

    bool warned() const;

    using ePlayerConquestEventBase::planArmyReturn;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdptr<eInvasionEvent> mInvasionEvent;
};

#endif // EPLAYERCONQUESTEVENT_H
