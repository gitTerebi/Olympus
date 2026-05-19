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

    void setGod(const eGodType god) { mGod = god; }
    eGodType god() const { return mGod; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eGodType mGod = eGodType::zeus;
};

#endif // EGODTRADERESUMESEVENT_H
