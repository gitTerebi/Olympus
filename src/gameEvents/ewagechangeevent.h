#ifndef EWAGECHANGEEVENT_H
#define EWAGECHANGEEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"

class SaveArchive;

class eWageChangeEvent : public eGameEvent,
                         public eCountEventValue {
public:
    eWageChangeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
};

#endif // EWAGECHANGEEVENT_H
