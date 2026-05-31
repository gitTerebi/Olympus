#ifndef EGODQUESTEVENT_H
#define EGODQUESTEVENT_H

#include "egodquesteventbase.h"

class eSaveArchive;

class eGodQuestEvent : public eGodQuestEventBase {
public:
    eGodQuestEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   GameBoard& board);
    ~eGodQuestEvent();

    void trigger() override;
    std::string longName() const override;
    bool finished() const override;

    void fulfill();
    void fulfilled();

    eEventTrigger& fulfilledTrigger() { return *mFulfilledTrigger; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdsptr<eEventTrigger> mFulfilledTrigger;
    bool mFulfilled = false;
};

#endif // EGODQUESTEVENT_H
