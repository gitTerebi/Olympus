#ifndef CHARACTER_FINISH_ACTIONS_H
#define CHARACTER_FINISH_ACTIONS_H

#include "character-action-function.h"
#include "pointers/estdpointer.h"

class eCharacter;
class eSaveArchive;

class eChar_fightFinish : public eCharActFunc {
public:
    eChar_fightFinish(GameBoard& board);
    eChar_fightFinish(GameBoard& board, eCharacter* const t);

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdptr<eCharacter> mTptr;
};

class eChar_killWithCorpseFinish : public eCharActFunc {
public:
    eChar_killWithCorpseFinish(GameBoard& board);
    eChar_killWithCorpseFinish(GameBoard& board, eCharacter* const t,
                               const bool withCorpse = false);

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mWithCorpse;
    stdptr<eCharacter> mTptr;
};

#endif // CHARACTER_FINISH_ACTIONS_H
