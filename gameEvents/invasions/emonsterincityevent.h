#ifndef EMONSTERINCITYEVENT_H
#define EMONSTERINCITYEVENT_H

#include "emonsterinvasioneventbase.h"

class eMonsterInCityEvent : public eMonsterInvasionEventBase {
public:
    eMonsterInCityEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
};

#endif // EMONSTERINCITYEVENT_H
