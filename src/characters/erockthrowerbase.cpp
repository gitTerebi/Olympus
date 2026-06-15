#include "erockthrowerbase.h"

#include "numbers.h"

eRockThrowerBase::eRockThrowerBase(GameBoard& board,
                                   const eCharTexs charTexs,
                                   const eCharacterType type) :
    eRangeSoldier(board, charTexs, type, Numbers::sRabbleRange) {
    setAttack(Numbers::sRabbleAttack);
    setHP(Numbers::sRabbleHP);
}
