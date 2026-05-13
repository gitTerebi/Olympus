#ifndef MONSTER_INVASION_EVENT_BASE_H
#define MONSTER_INVASION_EVENT_BASE_H

#include "../egameevent.h"
#include "../epointeventvalue.h"
#include "../emonsterseventvalue.h"

#include "characters/monsters/emonster.h"

class eSaveArchive;

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
    void serializeJson(eJsonArchive& ar) override {
        eGameEvent::serializeJson(ar);
        ePointEventValue::serializeJson(ar);
        eMonstersEventValue::serializeJson(ar);
        ar.field("mChooseMonster", mChooseMonster);
        ar.field("mAggressivness", mAggressivness);
        ar.field("mValid", mValid);
        int ns = ar.writing() ? (int)mSpawned.size() : 0;
        ar.field("ns", ns);
        if(ar.reading()) mSpawned.clear();
        for(int i = 0; i < ns; i++) {
            eMonsterType s = ar.writing() ? mSpawned[i] : eMonsterType::calydonianBoar;
            ar.field(("mSpawned" + std::to_string(i)).c_str(), s);
            if(ar.reading()) mSpawned.push_back(s);
        }
        int nk = ar.writing() ? (int)mKilled.size() : 0;
        ar.field("nk", nk);
        if(ar.reading()) mKilled.clear();
        for(int i = 0; i < nk; i++) {
            eMonsterType k = ar.writing() ? mKilled[i] : eMonsterType::calydonianBoar;
            ar.field(("mKilled" + std::to_string(i)).c_str(), k);
            if(ar.reading()) mKilled.push_back(k);
        }
    }

private:
    void serialize(eSaveArchive& ar);

    stdsptr<eEventTrigger> mKilledTrigger;
    bool mChooseMonster = false;
    eMonsterAggressivness mAggressivness = eMonsterAggressivness::passive;

    bool mValid = false;
    std::vector<eMonsterType> mKilled;
    std::vector<eMonsterType> mSpawned;
};

#endif // MONSTER_INVASION_EVENT_BASE_H
