#ifndef EGODTRADERESUMESEVENT_H
#define EGODTRADERESUMESEVENT_H

#include "gameEvents/egameevent.h"

#include "characters/gods/egod.h"

class eSaveArchive;

class eGodTradeResumesEvent : public eGameEvent {
public:
    eGodTradeResumesEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void setGod(const eGodType god) { mGod = god; }
    eGodType god() const { return mGod; }
void serializeJson(eJsonArchive& ar) override {
        eGameEvent::serializeJson(ar);
        ar.field("mGod", mGod);
}

private:
    void serialize(eSaveArchive& ar);
    eGodType mGod = eGodType::zeus;
};

#endif // EGODTRADERESUMESEVENT_H
