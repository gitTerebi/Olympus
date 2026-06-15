#ifndef ETRIREMEACTION_H
#define ETRIREMEACTION_H

#include "fighting-action.h"

class eTriremeWharf;
class SaveArchive;

enum class eTriremeActionStage {
    idle, home, abroad
};

class eTriremeAction : public FightingAction {
public:
    eTriremeAction(eTriremeWharf * const home,
                   eCharacter* const trireme);

    bool decide() override;

    void increment(const int by) override;

    void goHome() override;
    void goAbroad() override;

    eTriremeWharf* home() const;
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    eTile* exitPoint() const;
    void markLeaving();

    stdptr<eTriremeWharf> mHome;
    eTriremeActionStage mStage = eTriremeActionStage::idle;
    bool mLeavingNotified = false;
};

#endif // ETRIREMEACTION_H
