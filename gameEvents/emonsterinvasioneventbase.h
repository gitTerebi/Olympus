#ifndef EMONSTERINVASIONEVENTBASE_H
#define EMONSTERINVASIONEVENTBASE_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "emonsterseventvalue.h"

#include "characters/monsters/emonster.h"

class eMonsterInvasionEventBase : public eGameEvent,
                                  public ePointEventValue,
                                  public eMonstersEventValue {
public:
    eMonsterInvasionEventBase(const eCityId cid,
                              const eGameEventType type,
                              const eGameEventBranch branch,
                              eGameBoard& board);
    ~eMonsterInvasionEventBase();

    eMonsterAggressivness aggressivness() const
    { return mAggressivness; }
    void setAggressivness(const eMonsterAggressivness a)
    { mAggressivness = a; }

    void chooseMonster();
    bool valid() const { return mValid; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    bool finished() const override;

    void killed(const eMonsterType monster);

    eEventTrigger& killedTrigger() { return *mKilledTrigger; }
protected:
    eMonster* triggerBase();
private:
    stdsptr<eEventTrigger> mKilledTrigger;
    bool mChooseMonster = false;
    eMonsterAggressivness mAggressivness = eMonsterAggressivness::passive;

    bool mValid = false;
    std::vector<eMonsterType> mKilled;
    std::vector<eMonsterType> mSpawned;
};

#endif // EMONSTERINVASIONEVENTBASE_H
