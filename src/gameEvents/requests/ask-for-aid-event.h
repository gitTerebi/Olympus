#ifndef ASK_FOR_AID_EVENT_H
#define ASK_FOR_AID_EVENT_H

#include "../egameevent.h"

class SaveArchive;

class AskForAidEvent : public eGameEvent {
public:
    AskForAidEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void setCity(const stdsptr<WorldCity>& c) { mCity = c; }
    const stdsptr<WorldCity>& city() const { return mCity; }

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setArrivalDate(const eDate& d) { mArrivalDate = d; }
    const eDate& arrivalDate() const { return mArrivalDate; }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eDate mArrivalDate;
    bool mEnd = false;
    stdsptr<WorldCity> mCity;
};

#endif // ASK_FOR_AID_EVENT_H
