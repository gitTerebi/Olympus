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
                      eGameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setDuration(const int d) { mDuration = d; }
    int duration() const { return mDuration; }
    void serializeJson(eJsonArchive& ar) override {
        eGameEvent::serializeJson(ar);
        eGodEventValue::serializeJson(ar);
        eCityEventValue::serializeJson(ar, *gameBoard());
        ar.field("mDuration", mDuration);
        ar.field("mEnd", mEnd);
    }

private:
    void serialize(eSaveArchive& ar);

    int mDuration = 180;
    bool mEnd = false;
};

#endif // EGODDISASTEREVENT_H
