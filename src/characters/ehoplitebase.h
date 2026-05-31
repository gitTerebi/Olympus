#ifndef EHOPLITEBASE_H
#define EHOPLITEBASE_H

#include "esoldier.h"

class eHopliteBase : public eSoldier {
public:
    using eCharTexs = eFightingCharacterTextures eCharacterTextures::*;
    eHopliteBase(GameBoard& board,
                 const eCharTexs charTexs,
                 const eCharacterType type);
};

#endif // EHOPLITEBASE_H
