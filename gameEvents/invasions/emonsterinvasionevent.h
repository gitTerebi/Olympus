#ifndef EMONSTERINVASIONEVENT_H
#define EMONSTERINVASIONEVENT_H

#include "emonsterinvasioneventbase.h"

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
                            eGameBoard& board,
                            const eMonsterInvasionWarningType type);

    void trigger() override;
private:
    const eMonsterInvasionWarningType mType;
};

class eMonsterInvasionEvent : public eMonsterInvasionEventBase {
public:
    eMonsterInvasionEvent(const eCityId cid,
                          const eGameEventBranch branch,
                          eGameBoard& board);

    void pointerCreated() override;

    void setWarningMonths(const int ms) override;

    void trigger() override;
    std::string longName() const override;
private:
    eMonsterInvasionWarning* mInitialWarning = nullptr;
};

#endif // EMONSTERINVASIONEVENT_H
