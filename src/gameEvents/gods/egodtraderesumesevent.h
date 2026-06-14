#ifndef EGODTRADERESUMESEVENT_H
#define EGODTRADERESUMESEVENT_H

#include "gameEvents/egameevent.h"

#include "characters/gods/god.h"

class eSaveArchive;

class eGodTradeResumesEvent : public eGameEvent {
public:
    eGodTradeResumesEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void setGod(const GodType god) { mGod = god; }
    GodType god() const { return mGod; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    GodType mGod = GodType::zeus;
};

#endif // EGODTRADERESUMESEVENT_H
