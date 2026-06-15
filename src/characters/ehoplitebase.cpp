#include "ehoplitebase.h"

#include "numbers.h"

eHopliteBase::eHopliteBase(GameBoard& board,
                           const eCharTexs charTexs,
                           const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setAttack(Numbers::sHopliteAttack);
    setHP(Numbers::sHopliteHP);
}
