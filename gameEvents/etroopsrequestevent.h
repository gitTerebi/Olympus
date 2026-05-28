#ifndef ETROOPSREQUESTEVENT_H
#define ETROOPSREQUESTEVENT_H

#include "egameevent.h"
#include "ecityeventvalue.h"
#include "emonstereventvalue.h"
#include "eattackingcityeventvalue.h"

struct eReason;
class eSaveArchive;

enum class eTroopsRequestEventType {
    cityUnderAttack,
    cityAttacksRival,
    greekCityTerrorized
};

enum class eTroopsRequestEventEffect {
    unaffected,
    destroyed,
    conquered
};

class eTroopsRequestEvent : public eGameEvent,
                            public eCityEventValue,
                            public eMonsterEventValue,
                            public eAttackingCityEventValue {
public:
    eTroopsRequestEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);
   ~eTroopsRequestEvent();

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    std::string longName() const override;

    using eAction = std::function<void()>;
    void dispatch(const eAction& close = nullptr);

    void setType(const eTroopsRequestEventType t) { mType = t; }
    eTroopsRequestEventType type() const { return mType; }

    void setEffect(const eTroopsRequestEventEffect e) { mEffect = e; }
    eTroopsRequestEventEffect effect() const { return mEffect; }

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

    void set(eTroopsRequestEvent& src,
             const int postpone,
             const bool finish = false);

    eTroopsRequestEventType mType = eTroopsRequestEventType::cityUnderAttack;
    eTroopsRequestEventEffect mEffect = eTroopsRequestEventEffect::unaffected;

    bool mFinish = false;
    int mPostpone = 0;

    stdsptr<eEventTrigger> mEarlyTrigger;
    stdsptr<eEventTrigger> mComplyTrigger;
    stdsptr<eEventTrigger> mTooLateTrigger;
    stdsptr<eEventTrigger> mRefuseTrigger;
    stdsptr<eEventTrigger> mLostBattleTrigger;
};

#endif // ETROOPSREQUESTEVENT_H
