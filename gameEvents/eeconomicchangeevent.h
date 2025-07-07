#ifndef EECONOMICCHANGEEVENT_H
#define EECONOMICCHANGEEVENT_H

#include "eeconomicmilitarychangeeventbase.h"

class eEconomicChangeEvent : public eEconomicMilitaryChangeEventBase {
public:
    eEconomicChangeEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);

   void trigger() override;
   std::string longName() const override;
};

#endif // EECONOMICCHANGEEVENT_H
