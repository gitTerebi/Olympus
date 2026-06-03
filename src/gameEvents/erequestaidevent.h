#ifndef EREQUESTAIDEVENT_H
#define EREQUESTAIDEVENT_H

#include "egameevent.h"

class eSaveArchive;

class eRequestAidEvent : public eGameEvent {
public:
    eRequestAidEvent(const eCityId cid,
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
    void serializeFields(eSaveArchive& ar) override;
private:
    eDate mArrivalDate;
    bool mEnd = false;
    stdsptr<WorldCity> mCity;
};

#endif // EREQUESTAIDEVENT_H
