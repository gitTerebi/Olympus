#ifndef SEND_TROOPS_EVENT_H
#define SEND_TROOPS_EVENT_H

#include "../egameevent.h"
#include "../ecityeventvalue.h"
#include "../emonstereventvalue.h"
#include "../eattackingcityeventvalue.h"

struct eReason;
class eSaveArchive;

enum class SendTroopsEventType {
    cityUnderAttack,
    cityAttacksRival,
    greekCityTerrorized
};

enum class SendTroopsEventEffect {
    unaffected,
    destroyed,
    conquered
};

class SendTroopsEvent : public eGameEvent,
                            public eCityEventValue,
                            public eMonsterEventValue,
                            public eAttackingCityEventValue {
public:
    SendTroopsEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);
   ~SendTroopsEvent();

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    std::string longName() const override;

    using eAction = std::function<void()>;
    void dispatch(const eAction& close = nullptr);

    void setType(const SendTroopsEventType t) { mType = t; }
    SendTroopsEventType type() const { return mType; }

    void setEffect(const SendTroopsEventEffect e) { mEffect = e; }
    SendTroopsEventEffect effect() const { return mEffect; }

    void won();
    void lost();
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    enum class eResponse {
        dispatchNow,
        postpone,
        refuse
    };

    void postponeResponse();
    void refuse();

    void finished(eEventTrigger& t, const eReason& r);

    void set(SendTroopsEvent& src,
             const int postpone,
             const bool finish = false);

    SendTroopsEventType mType = SendTroopsEventType::cityUnderAttack;
    SendTroopsEventEffect mEffect = SendTroopsEventEffect::unaffected;

    bool mFinish = false;
    int mPostpone = 0;

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
    stdsptr<eEventTrigger> mLostBattleTrigger;
};

#endif // SEND_TROOPS_EVENT_H
