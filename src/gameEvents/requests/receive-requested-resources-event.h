#ifndef RECEIVE_REQUESTED_RESOURCES_EVENT_H
#define RECEIVE_REQUESTED_RESOURCES_EVENT_H

#include "../eresourcegrantedeventbase.h"

class ReceiveRequestedResourcesEvent : public eResourceGrantedEventBase {
public:
    ReceiveRequestedResourcesEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    void initialize(const bool postpone,
                    const eResourceType res,
                    const stdsptr<WorldCity>& c);

    std::string longName() const override;
};

#endif // RECEIVE_REQUESTED_RESOURCES_EVENT_H
