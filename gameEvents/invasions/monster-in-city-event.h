#ifndef MONSTER_IN_CITY_EVENT_H
#define MONSTER_IN_CITY_EVENT_H

#include "monster-invasion-event-base.h"

class eMonsterInCityEvent : public eMonsterInvasionEventBase {
public:
    eMonsterInCityEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
};

#endif // MONSTER_IN_CITY_EVENT_H
