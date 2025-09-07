#ifndef EPLAYERCONQUESTEVENTBASE_H
#define EPLAYERCONQUESTEVENTBASE_H

#include "earmyeventbase.h"

class ePlayerConquestEventBase : public eArmyEventBase {
public:
    ePlayerConquestEventBase(const eCityId cid,
                             const eGameEventType type,
                             const eGameEventBranch branch,
                             eGameBoard& board);
    ~ePlayerConquestEventBase();

    void addAres();
protected:
    void removeConquestEvent();
};

#endif // EPLAYERCONQUESTEVENTBASE_H
