#ifndef MONSTER_UNLEASHED_EVENT_H
#define MONSTER_UNLEASHED_EVENT_H

#include "monster-invasion-event-base.h"

class eMonsterUnleashedEvent : public eMonsterInvasionEventBase {
public:
    eMonsterUnleashedEvent(const eCityId cid,
                           const eGameEventBranch branch,
                           eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
};

#endif // MONSTER_UNLEASHED_EVENT_H
