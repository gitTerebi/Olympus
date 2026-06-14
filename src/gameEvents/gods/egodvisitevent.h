#ifndef EGODVISITEVENT_H
#define EGODVISITEVENT_H

#include "gameEvents/egameevent.h"
#include "characters/gods/god.h"

class eSaveArchive;

class eGodVisitEvent : public eGameEvent {
public:
    eGodVisitEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   GameBoard& board);

    const std::vector<GodType>& types() const { return mTypes; }
    void setTypes(const std::vector<GodType>& types);

    void setRandom(const bool r);

    void loadResources() const override;

    void trigger() override;
    std::string longName() const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    std::vector<GodType> mTypes;
    int mNextId = 0;
    bool mRandom = false;
};

#endif // EGODVISITEVENT_H
