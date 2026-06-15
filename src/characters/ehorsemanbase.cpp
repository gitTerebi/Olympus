#include "ehorsemanbase.h"

#include "numbers.h"

eHorsemanBase::eHorsemanBase(GameBoard& board,
                             const eCharTexs charTexs,
                             const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setAttack(Numbers::sHorsemanAttack);
    setHP(Numbers::sHorsemanHP);
}
