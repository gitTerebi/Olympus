#ifndef EMILITARYCHANGEEVENT_H
#define EMILITARYCHANGEEVENT_H

#include "eeconomicmilitarychangeeventbase.h"

class eMilitaryChangeEvent : public eEconomicMilitaryChangeEventBase {
public:
    eMilitaryChangeEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);

   void trigger() override;
   std::string longName() const override;;
};

#endif // EMILITARYCHANGEEVENT_H
