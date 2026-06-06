#ifndef ERAIDRESOURCEEVENT_H
#define ERAIDRESOURCEEVENT_H

#include "../eresourcegrantedeventbase.h"

class RaidResourceEvent : public eResourceGrantedEventBase {
public:
    RaidResourceEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    std::string longName() const override;
};

#endif // ERAIDRESOURCEEVENT_H
