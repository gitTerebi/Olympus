#include "ehoplitebase.h"

#include "enumbers.h"

eHopliteBase::eHopliteBase(GameBoard& board,
                           const eCharTexs charTexs,
                           const eCharacterType type) :
    eSoldier(board, charTexs, type) {
    setAttack(eNumbers::sHopliteAttack);
    setHP(eNumbers::sHopliteHP);
}
