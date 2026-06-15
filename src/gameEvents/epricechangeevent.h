#ifndef EPRICECHANGEEVENT_H
#define EPRICECHANGEEVENT_H

#include "egameevent.h"
#include "eresourceeventvalue.h"
#include "ecounteventvalue.h"

class ePriceChangeEvent : public eGameEvent,
                          public eResourceEventValue,
                          public eCountEventValue {
public:
    ePriceChangeEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // EPRICECHANGEEVENT_H
