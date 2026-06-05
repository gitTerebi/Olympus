#ifndef FULFILL_REQUEST_EVENT_H
#define FULFILL_REQUEST_EVENT_H

#include "../egameevent.h"
#include "../ecounteventvalue.h"
#include "../eresourceeventvalue.h"
#include "../ecityeventvalue.h"
#include "../gods/egodeventvalue.h"

struct eReason;
struct eCityRequest;
struct eEventData;
class eSaveArchive;

enum class ReceiveRequestType {
    tribute,
    famine,
    general,
    project,
    festival,
    financialWoes
};

enum class ReceiveRequestResult {
    comply,
    tooLate,
    refuse
};

class FulfillRequestEvent : public eGameEvent,
                             public eResourceEventValue,
                             public eCountEventValue,
                             public eCityEventValue,
                             public eGodEventValue {
public:
    FulfillRequestEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         GameBoard& board);
    ~FulfillRequestEvent();

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    std::string longName() const override;

    eCityRequest cityRequest() const;
    void dispatch(const eCityId cid);
    void finish(ReceiveRequestResult result);

    void setRequestType(const ReceiveRequestType t)
    { mRequestType = t; }
    ReceiveRequestType requestType() const
    { return mRequestType; }

    std::string requestInfo(int stock, const eDate& currentDate) const;
    std::string dispatchText(int stock, const eDate& currentDate) const;
    std::string overdueStatusText(const eDate& currentDate) const;
    bool finished() const override;
    bool isOverdue(const eDate& currentDate) const;
    void advanceIfNeeded(const eDate& currentDate);
    bool isPostponed() const;
    bool isActiveCityRequest() const;
    int requestId() const { return mRequestId; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    enum class eResponse {
        dispatch,
        postpone,
        refuse
    };

    bool startRequest(GameBoard& board);
    bool startQueuedRequest(GameBoard& board);
    bool initializeRequest(GameBoard& board);
    eEventData createEventData(GameBoard& board) const;
    void showRequestFinished(GameBoard& board, eEventData& ed);
    void showRequestPopup(GameBoard& board, eEventData& ed);
    void addFulfillButton(GameBoard& board, eEventData& ed);
    void addDrachmasFulfillButton(GameBoard& board, eEventData& ed);
    void addResourceFulfillButtons(GameBoard& board, eEventData& ed);
    void addPostponeButton(GameBoard& board, eEventData& ed);
    void addRefuseButton(GameBoard& board, eEventData& ed);
    void addRequestToSidePanel(GameBoard& board);
    void finished(eEventTrigger& t, const eReason& r);
    void advanceToNextStep(GameBoard& board);
    eDate complyDate() const;
    int remainingMonths(const eDate& deadline, const eDate& current) const;

    void postpone();
    int complyMonths() const;



    ReceiveRequestType mRequestType = ReceiveRequestType::general;
    ReceiveRequestResult mRequestResult = ReceiveRequestResult::comply;

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

#endif // FULFILL_REQUEST_EVENT_H
