#ifndef EFIGHTACTION_H
#define EFIGHTACTION_H

#include "character-action.h"
#include "characters/echaracter.h"

class eSaveArchive;

class eFightAction : public eCharacterAction {
public:
    eFightAction(eCharacter* const c,
                 eCharacter* const o = nullptr);

    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:

    stdptr<eCharacter> mOpponent;

    int mTime = 0;
};

#endif // EFIGHTACTION_H
