#ifndef EWAGECHANGEEVENT_H
#define EWAGECHANGEEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"

class eSaveArchive;

class eWageChangeEvent : public eGameEvent,
                         public eCountEventValue {
public:
    eWageChangeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     eGameBoard& board);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
};

#endif // EWAGECHANGEEVENT_H
