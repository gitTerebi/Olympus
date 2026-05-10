#ifndef EARESHELPACTION_H
#define EARESHELPACTION_H

#include "characters/gods/actions/egodaction.h"

class eSaveArchive;

enum class eAresHelpStage {
    none, appear, go
};

class eAresHelpAction : public eGodAction {
public:
    eAresHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    static bool sHelpNeeded(const ePlayerId pid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);
    void goToTarget();

    eAresHelpStage mStage{eAresHelpStage::none};
};

#endif // EARESHELPACTION_H
