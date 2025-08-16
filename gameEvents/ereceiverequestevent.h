#ifndef ERECEIVEREQUESTEVENT_H
#define ERECEIVEREQUESTEVENT_H

#include "eresourcecountcityevent.h"

#include "characters/gods/egod.h"

struct eReason;
struct eCityRequest;

enum class eReceiveRequestType {
    tribute,
    famine,
    general,
    project,
    festival,
    financialWoes
};

class eReceiveRequestEvent : public eResourceCountCityEvent {
public:
    eReceiveRequestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);
    ~eReceiveRequestEvent();

    void initialize(const int postpone,
                    const eResourceType res,
                    const int count,
                    const stdsptr<eWorldCity>& c,
                    const bool finish = false);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    eCityRequest cityRequest() const;
    void dispatch(const eCityId cid);
    void fulfillWithoutCost();

    void setRequestType(const eReceiveRequestType t)
    { mRequestType = t; }
    eReceiveRequestType requestType() const
    { return mRequestType; }

    void setGod(const eGodType god) { mGod = god; }
    eGodType god() const { return mGod; }
private:
    void finished(eEventTrigger& t, const eReason& r);

    eReceiveRequestType mRequestType = eReceiveRequestType::general;

    bool mFinish = false;
    int mPostpone = 0;

    eGodType mGod = eGodType::zeus;

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
};

#endif // ERECEIVEREQUESTEVENT_H
