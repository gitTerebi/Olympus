#ifndef EGIFTTOEVENT_H
#define EGIFTTOEVENT_H

#include "egameevent.h"

#include "engine/e-worldcity.h"

class eSaveArchive;

class eGiftToEvent : public eGameEvent {
public:
    eGiftToEvent(const eCityId cid,
                 const eGameEventBranch branch,
                 eGameBoard& board);

    void initialize(const stdsptr<eWorldCity>& c,
                    const eResourceType type,
                    const int count);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override ;
    void read(eReadStream& src) override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    stdsptr<eWorldCity> mCity;
    eResourceType mResource;
    int mCount;
};

#endif // EGIFTTOEVENT_H
