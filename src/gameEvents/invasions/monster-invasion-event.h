#ifndef MONSTER_INVASION_EVENT_H
#define MONSTER_INVASION_EVENT_H

#include "monster-invasion-event-base.h"

enum class eMonsterInvasionWarningType {
    warningInitial,
    warning24,
    warning12,
    warning6,
    warning1
};

class eMonsterInvasionEvent;

class eMonsterInvasionWarning : public eWarning {
public:
    eMonsterInvasionWarning(const int warningDays,
                            eGameEvent& parent,
                            const eCityId cid,
                            GameBoard& board,
                            const eMonsterInvasionWarningType type);

    void trigger() override;
private:
    const eMonsterInvasionWarningType mType;
};

class eMonsterInvasionEvent : public eMonsterInvasionEventBase {
public:
    eMonsterInvasionEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          GameBoard& board);

    void pointerCreated() override;

    void setWarningMonths(const int ms) override;

    void trigger() override;
    std::string longName() const override;
private:
    eMonsterInvasionWarning* mInitialWarning = nullptr;
};

#endif // MONSTER_INVASION_EVENT_H
