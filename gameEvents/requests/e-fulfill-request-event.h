#ifndef E_FULFILL_REQUEST_EVENT_H
#define E_FULFILL_REQUEST_EVENT_H

#include "../egameevent.h"
#include "../ecounteventvalue.h"
#include "../eresourceeventvalue.h"
#include "../ecityeventvalue.h"
#include "../gods/egodeventvalue.h"

struct eReason;
struct eCityRequest;
struct eEventData;
class eSaveArchive;

enum class eReceiveRequestType {
    tribute,
    famine,
    general,
    project,
    festival,
    financialWoes
};

enum class eReceiveRequestResult {
    comply,
    tooLate,
    refuse
};

class eFulfillRequestEvent : public eGameEvent,
                             public eResourceEventValue,
                             public eCountEventValue,
                             public eCityEventValue,
                             public eGodEventValue {
public:
    eFulfillRequestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         eGameBoard& board);
    ~eFulfillRequestEvent();

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    eCityRequest cityRequest() const;
    void dispatch(const eCityId cid);
    void finish(eReceiveRequestResult result);

    void setRequestType(const eReceiveRequestType t)
    { mRequestType = t; }
    eReceiveRequestType requestType() const
    { return mRequestType; }

    std::string requestInfo(int stock, const eDate& currentDate) const;
    std::string dispatchText(int stock, const eDate& currentDate) const;
    std::string overdueStatusText(const eDate& currentDate) const;
    bool finished() const override;
    bool isOverdue(const eDate& currentDate) const;
    bool isPostponed() const;
    bool isActiveCityRequest() const;
    int requestId() const { return mRequestId; }

private:
    void serialize(eSaveArchive& ar);

    bool startRequest(eGameBoard& board);
    bool startQueuedRequest(eGameBoard& board);
    bool initializeRequest(eGameBoard& board);
    eEventData createEventData(eGameBoard& board) const;
    void showRequestFinished(eGameBoard& board, eEventData& ed);
    void showRequestPopup(eGameBoard& board, eEventData& ed);
    void addFulfillButton(eGameBoard& board, eEventData& ed);
    void addDrachmasFulfillButton(eGameBoard& board, eEventData& ed);
    void addResourceFulfillButtons(eGameBoard& board, eEventData& ed);
    void addPostponeButton(eGameBoard& board, eEventData& ed);
    void addRefuseButton(eGameBoard& board, eEventData& ed);
    void addRequestToSidePanel(eGameBoard& board);
    void finished(eEventTrigger& t, const eReason& r);
    eDate complyDate() const;
    int remainingMonths(const eDate& deadline, const eDate& current) const;

    void postpone();
    void scheduleStep(const int step, const eDate& date);
    void copyFrom(const eFulfillRequestEvent& src, const int step);
    int complyMonths() const;



    eReceiveRequestType mRequestType = eReceiveRequestType::general;
    eReceiveRequestResult mRequestResult = eReceiveRequestResult::comply;

    bool mRequestFinished = false;
    int mRequestStep = 0;
    int mRequestId = 0;
    eDate mRequestDate{1, eMonth::january, 1};
    eDate mRequestDeadline{1, eMonth::january, 1};

    int mComplyStep = 0;
    bool mPostponed = false;
    eDate mComplyStartDate{1, eMonth::january, 1};

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
};

#endif // E_FULFILL_REQUEST_EVENT_H
