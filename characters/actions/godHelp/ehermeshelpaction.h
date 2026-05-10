#ifndef EHERMESHELPACTION_H
#define EHERMESHELPACTION_H

#include "characters/gods/actions/egodaction.h"

class eSaveArchive;

enum class eHermesHelpStage {
    none, appear, provide, disappear
};

class eHermesHelpAction : public eGodAction {
public:
    eHermesHelpAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    static bool sHelpNeeded(const ePlayerId pid,
                            const eGameBoard& board);
private:
    void serialize(eSaveArchive& ar);
    void provide();

    eHermesHelpStage mStage{eHermesHelpStage::none};
};

#endif // EHERMESHELPACTION_H
