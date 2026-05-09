#ifndef ERECEIVETRIBUTEEVENT_H
#define ERECEIVETRIBUTEEVENT_H

#include "egameevent.h"

#include "engine/e-worldcity.h"

class eSaveArchive;

class eReceiveTributeEvent : public eGameEvent {
public:
    eReceiveTributeEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);

    void initialize(const stdsptr<eWorldCity>& c);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
private:
    void serialize(eSaveArchive& ar);

    stdsptr<eWorldCity> mCity;
};

#endif // ERECEIVETRIBUTEEVENT_H
