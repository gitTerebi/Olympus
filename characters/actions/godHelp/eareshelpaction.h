#ifndef EARESHELPACTION_H
#define EARESHELPACTION_H

#include "characters/gods/actions/god-action.h"

class eSaveArchive;

enum class eAresHelpStage {
    none, appear, go
};

class eAresHelpAction : public eGodAction {
public:
    eAresHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const ePlayerId pid,
                            const GameBoard& board);
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goToTarget();

    eAresHelpStage mStage{eAresHelpStage::none};
};

#endif // EARESHELPACTION_H
