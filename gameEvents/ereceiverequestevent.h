#ifndef ERECEIVEREQUESTEVENT_H
#define ERECEIVEREQUESTEVENT_H

#include "egameevent.h"
#include "ecounteventvalue.h"
#include "eresourceeventvalue.h"
#include "ecityeventvalue.h"
#include "egodeventvalue.h"

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

class eReceiveRequestEvent : public eGameEvent,
                             public eResourceEventValue,
                             public eCountEventValue,
                             public eCityEventValue,
                             public eGodEventValue {
public:
    eReceiveRequestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);
    ~eReceiveRequestEvent();

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

    void initialize(const int postpone,
                    const eResourceType res,
                    const int count,
                    const stdsptr<eWorldCity> &c,
                    const bool finish);
private:
    void set(eReceiveRequestEvent& src,
             const int postpone,
             const bool finish = false);

    void finished(eEventTrigger& t, const eReason& r);

    eReceiveRequestType mRequestType = eReceiveRequestType::general;

    bool mFinish = false;
    int mPostpone = 0;

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
};

#endif // ERECEIVEREQUESTEVENT_H
