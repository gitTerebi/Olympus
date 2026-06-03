#ifndef EPAYTRIBUTEEVENT_H
#define EPAYTRIBUTEEVENT_H

#include "../egameevent.h"

#include "engine/world-city.h"
#include "engine/ecityrequest.h"
#include "engine/eevent.h"
#include "engine/eresourcetype.h"

class eSaveArchive;

enum class ePayTributeResult {
    comply,
    tooLate,
    refuse
};

class ePayTributeEvent : public eGameEvent {
public:
    ePayTributeEvent(const eCityId cid,
                     const eGameEventBranch branch,
                     GameBoard& board);
    ~ePayTributeEvent();

    void initialize(const stdsptr<WorldCity>& c);

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    std::string longName() const override;
    bool finished() const override;

    eCityRequest cityRequest() const;
    void dispatch(eCityId cid);
    void postpone();
    std::string dispatchText(int stock, const eDate& currentDate) const;
    std::string overdueStatusText(const eDate& currentDate) const;
    void advanceIfNeeded(const eDate& currentDate);
    bool isPostponed() const;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    enum class eResponse {
        dispatch,
        postpone,
        refuse
    };

    void activate();
    void advanceToNextStep(GameBoard& board);
    eEvent stepEvent() const;
    int complyMonths() const;
    int popupComplyMonths() const;
    void finish(ePayTributeResult result);
    int remainingMonths(const eDate& deadline, const eDate& current) const;

    stdsptr<WorldCity> mCity;
    eResourceType mResource = eResourceType::drachmas;
    int mCount = 0;
    bool mActive = false;
    bool mPostponed = false;
    eEvent mEvent = eEvent::generalRequestTributeInitial;
    eEvent mComplyEvent = eEvent::generalRequestTributeInitial;
    int mRequestStep = 0;
    eDate mRequestDate{1, eMonth::january, 1};
    eDate mRequestDeadline{1, eMonth::january, 1};
    eDate mComplyStartDate{1, eMonth::january, 1};
};

#endif // EPAYTRIBUTEEVENT_H
