#ifndef ETRIREMEACTION_H
#define ETRIREMEACTION_H

#include "efightingaction.h"

class eTriremeWharf;
class eSaveArchive;

enum class eTriremeActionStage {
    idle, home, abroad
};

class eTriremeAction : public eFightingAction {
public:
    eTriremeAction(eTriremeWharf * const home,
                   eCharacter* const trireme);

    bool decide() override;

    void increment(const int by) override;

    void goHome() override;
    void goAbroad() override;

    eTriremeWharf* home() const;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    eTile* exitPoint() const;
    void markLeaving();

    stdptr<eTriremeWharf> mHome;
    eTriremeActionStage mStage = eTriremeActionStage::idle;
    bool mLeavingNotified = false;
};

#endif // ETRIREMEACTION_H
