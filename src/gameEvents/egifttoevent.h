#ifndef EGIFTTOEVENT_H
#define EGIFTTOEVENT_H

#include "egameevent.h"

#include "engine/e-worldcity.h"

class eSaveArchive;

class eGiftToEvent : public eGameEvent {
public:
    eGiftToEvent(const eCityId cid,
                 const eGameEventBranch branch,
                 GameBoard& board);

    void initialize(const stdsptr<eWorldCity>& c,
                    const eResourceType type,
                    const int count);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdsptr<eWorldCity> mCity;
    eResourceType mResource{};
    int mCount = 0;
};

#endif // EGIFTTOEVENT_H
