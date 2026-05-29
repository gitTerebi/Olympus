#include "ehorsemanbase.h"

#include "enumbers.h"

eHorsemanBase::eHorsemanBase(GameBoard& board,
                             const eCharTexs charTexs,
                             const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setAttack(eNumbers::sHorsemanAttack);
    setHP(eNumbers::sHorsemanHP);
}
