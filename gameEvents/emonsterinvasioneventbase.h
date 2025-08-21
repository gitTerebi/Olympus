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

    eMonsterAggressivness aggressivness() const
    { return mAggressivness; }
    void setAggressivness(const eMonsterAggressivness a)
    { mAggressivness = a; }

    void setWarned(const bool w);

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    eMonster* triggerBase();
private:
    bool mWarned = false;
    eMonsterAggressivness mAggressivness = eMonsterAggressivness::passive;
};

#endif // EMONSTERINVASIONEVENTBASE_H
