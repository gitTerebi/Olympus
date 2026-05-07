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

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void trigger() override;
    std::string longName() const override;
private:
    void serialize(eSaveArchive& ar);
};

#endif // EWAGECHANGEEVENT_H
