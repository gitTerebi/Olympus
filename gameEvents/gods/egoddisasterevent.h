#ifndef EGODDISASTEREVENT_H
#define EGODDISASTEREVENT_H

#include "gameEvents/egameevent.h"

#include "gameEvents/gods/egodeventvalue.h"
#include "gameEvents/ecityeventvalue.h"

class eSaveArchive;

class eGodDisasterEvent : public eGameEvent,
                          public eGodEventValue,
                          public eCityEventValue {
public:
    eGodDisasterEvent(const eCityId cid,
                      const eGameEventBranch branch,
                      GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setDuration(const int d) { mDuration = d; }
    int duration() const { return mDuration; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mDuration = 180;
    bool mEnd = false;
};

#endif // EGODDISASTEREVENT_H
