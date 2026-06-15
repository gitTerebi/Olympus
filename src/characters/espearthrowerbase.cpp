#include "espearthrowerbase.h"

#include "numbers.h"

eSpearthrowerBase::eSpearthrowerBase(GameBoard& board,
                                     const eCharTexs charTexs,
                                     const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setRange(Numbers::sSpearthrowerRange);
    setAttack(Numbers::sSpearthrowerAttack);
    setHP(Numbers::sSpearthrowerHP);
}
