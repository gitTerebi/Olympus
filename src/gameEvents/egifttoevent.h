#ifndef EGIFTTOEVENT_H
#define EGIFTTOEVENT_H

#include "egameevent.h"

#include "engine/world-city.h"

class SaveArchive;

class eGiftToEvent : public eGameEvent {
public:
    eGiftToEvent(const eCityId cid,
                 const eGameEventBranch branch,
                 GameBoard& board);

    void initialize(const stdsptr<WorldCity>& c,
                    const eResourceType type,
                    const int count);

    void trigger() override;
    std::string longName() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdsptr<WorldCity> mCity;
    eResourceType mResource{};
    int mCount = 0;
};

#endif // EGIFTTOEVENT_H
