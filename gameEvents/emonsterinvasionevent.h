#ifndef EMONSTERINVASIONEVENT_H
#define EMONSTERINVASIONEVENT_H

#include "emonsterinvasioneventbase.h"

enum class eMonsterInvasionWarningType {
    warning36,
    warning24,
    warning12,
    warning6,
    warning1
};

class eMonsterInvasionEvent;

class eMonsterInvasionWarningEvent : public eGameEvent {
public:
    eMonsterInvasionWarningEvent(const eCityId cid,
                                 const eGameEventBranch branch,
                                 eGameBoard& board);

    void initialize(const eMonsterInvasionWarningType type,
                    const eMonsterType monster);

    void setMonster(const eMonsterType m);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override ;
    void read(eReadStream& src) override;
private:
    eMonsterInvasionWarningType mType;
    eMonsterType mMonster;
};

class eMonsterInvasionEvent : public eMonsterInvasionEventBase {
public:
    eMonsterInvasionEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          eGameBoard& board);

    void pointerCreated() override;

    void trigger() override;
    std::string longName() const override;
};

#endif // EMONSTERINVASIONEVENT_H
