#include "earcherbase.h"

#include "numbers.h"

eArcherBase::eArcherBase(GameBoard& board,
                         const eCharTexs charTexs,
                         const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setRange(Numbers::sArcherRange);
    setAttack(Numbers::sArcherAttack);
    setHP(Numbers::sArcherHP);
}
