#ifndef MONSTER_INVASION_EVENT_BASE_H
#define MONSTER_INVASION_EVENT_BASE_H

#include "../egameevent.h"
#include "../epointeventvalue.h"
#include "../emonsterseventvalue.h"

#include "characters/monsters/emonster.h"

class SaveArchive;

class eMonsterInvasionEventBase : public eGameEvent,
                                  public ePointEventValue,
                                  public eMonstersEventValue {
public:
    eMonsterInvasionEventBase(const eCityId cid,
                              const eGameEventType type,
                              const eGameEventBranch branch,
                              GameBoard& board);
    ~eMonsterInvasionEventBase();

    eMonsterAggressivness aggressivness() const
    { return mAggressivness; }
    void setAggressivness(const eMonsterAggressivness a)
    { mAggressivness = a; }

    void chooseMonster();
    bool valid() const { return mValid; }

    bool finished() const override;

    void killed(const eMonsterType monster);

    eEventTrigger& killedTrigger() { return *mKilledTrigger; }
protected:
    eMonster* triggerBase();
    void serializeFields(SaveArchive& ar) override;
private:
    stdsptr<eEventTrigger> mKilledTrigger;
    bool mChooseMonster = false;
    eMonsterAggressivness mAggressivness = eMonsterAggressivness::passive;

    bool mValid = false;
    std::vector<eMonsterType> mKilled;
    std::vector<eMonsterType> mSpawned;
};

#endif // MONSTER_INVASION_EVENT_BASE_H
