#ifndef ETROOPSREQUESTEVENT_H
#define ETROOPSREQUESTEVENT_H

#include "egameevent.h"
#include "ecityevent.h"
#include "emonstereventvalue.h"
#include "eattackingcityeventvalue.h"

struct eReason;

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
                            public eCityEvent,
                            public eMonsterEventValue,
                            public eAttackingCityEventValue {
public:
    eTroopsRequestEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        eGameBoard& board);
   ~eTroopsRequestEvent();

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    using eAction = std::function<void()>;
    void dispatch(const eAction& close = nullptr);

    void setType(const eTroopsRequestEventType t) { mType = t; }
    eTroopsRequestEventType type() const { return mType; }

    void setEffect(const eTroopsRequestEventEffect e) { mEffect = e; }
    eTroopsRequestEventEffect effect() const { return mEffect; }

    void won();
    void lost();
private:
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
