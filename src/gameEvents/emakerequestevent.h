#ifndef EMAKEREQUESTEVENT_H
#define EMAKEREQUESTEVENT_H

#include "eresourcegrantedeventbase.h"

class eMakeRequestEvent : public eResourceGrantedEventBase {
public:
    eMakeRequestEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    void initialize(const bool postpone,
                    const eResourceType res,
                    const stdsptr<WorldCity>& c);

    std::string longName() const override;
};

#endif // EMAKEREQUESTEVENT_H
