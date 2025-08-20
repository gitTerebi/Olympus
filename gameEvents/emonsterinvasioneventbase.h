#ifndef EMONSTERINVASIONEVENTBASE_H
#define EMONSTERINVASIONEVENTBASE_H

#include "egameevent.h"
#include "epointeventvalue.h"

#include "characters/monsters/emonster.h"

class eMonsterInvasionEventBase : public eGameEvent,
                                  public ePointEventValue {
public:
    eMonsterInvasionEventBase(const eCityId cid,
                              const eGameEventType type,
                              const eGameEventBranch branch,
                              eGameBoard& board);

    eMonsterType type() const { return mType; }
    void setType(const eMonsterType type);

    eMonsterAggressivness aggressivness() const
    { return mAggressivness; }
    void setAggressivness(const eMonsterAggressivness a)
    { mAggressivness = a; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    eMonster* triggerBase();
private:
    eMonsterType mType = eMonsterType::calydonianBoar;
    eMonsterAggressivness mAggressivness = eMonsterAggressivness::passive;
};

#endif // EMONSTERINVASIONEVENTBASE_H
