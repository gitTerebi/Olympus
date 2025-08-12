#ifndef ERECEIVEREQUESTEVENT_H
#define ERECEIVEREQUESTEVENT_H

#include "egameevent.h"

#include "engine/eworldcity.h"

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

class eReceiveRequestEvent : public eGameEvent {
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

    const stdsptr<eWorldCity>& city() const { return mCity; }
    void setCity(const stdsptr<eWorldCity>& c);

    eResourceType resourceType() const { return mResource; }
    int resourceCount() const { return mCount; }

    int minResourceCount() const { return mMinCount; }
    void setMinResourceCount(const int c) { mMinCount = c; }

    int maxResourceCount() const { return mMaxCount; }
    void setMaxResourceCount(const int c) { mMaxCount = c; }

    const std::vector<eResourceType>& resourceTypes() const
    { return mResources; }
    eResourceType resourceType(const int id) const
    { return mResources[id]; }
    void setResourceType(const int id, const eResourceType type)
    { mResources[id] = type; }

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

    void chooseTypeAndCount();

    eReceiveRequestType mRequestType = eReceiveRequestType::general;

    bool mFinish = false;
    int mPostpone = 0;

    std::vector<eResourceType> mResources = {eResourceType::fleece,
                                             eResourceType::fleece,
                                             eResourceType::fleece};
    int mMinCount = 8;
    int mMaxCount = 16;

    eResourceType mResource = eResourceType::fleece;
    int mCount = 16;

    eGodType mGod = eGodType::zeus;

    stdsptr<eWorldCity> mCity;

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
};

#endif // ERECEIVEREQUESTEVENT_H
