#ifndef ESUPPLYCHANGEEVENT_H
#define ESUPPLYCHANGEEVENT_H

#include "esupplydemandchangeevent.h"

class eSupplyChangeEvent : public eSupplyDemandChangeEvent {
public:
    eSupplyChangeEvent(const eGameEventBranch branch,
                       eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
};

#endif // ESUPPLYCHANGEEVENT_H
