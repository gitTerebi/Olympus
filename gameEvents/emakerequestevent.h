#ifndef EMAKEREQUESTEVENT_H
#define EMAKEREQUESTEVENT_H

#include "eresourcegrantedeventbase.h"

class eMakeRequestEvent : public eResourceGrantedEventBase {
public:
    eMakeRequestEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      eGameBoard& board);

    void initialize(const bool postpone,
                    const eResourceType res,
                    const stdsptr<eWorldCity>& c);

    std::string longName() const override;
};

#endif // EMAKEREQUESTEVENT_H
