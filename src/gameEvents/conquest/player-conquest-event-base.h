#ifndef EPLAYERCONQUESTEVENTBASE_H
#define EPLAYERCONQUESTEVENTBASE_H

#include "army-event-base.h"

class PlayerConquestEventBase : public ArmyEventBase {
public:
    PlayerConquestEventBase(const eCityId cid,
                            const eGameEventType type,
                            const eGameEventBranch branch,
                            GameBoard& board);
    ~PlayerConquestEventBase();

    void addAres();
protected:
    void removeConquestEvent();
};

#endif // EPLAYERCONQUESTEVENTBASE_H
