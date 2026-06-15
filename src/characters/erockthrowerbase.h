#ifndef EROCKTHROWERBASE_H
#define EROCKTHROWERBASE_H

#include "erangesoldier.h"

class eRockThrowerBase : public eRangeSoldier {
public:
    using eCharTexs = RockThrowerTextures CharacterTextures::*;
    eRockThrowerBase(GameBoard& board,
                     const eCharTexs charTexs,
                     const eCharacterType type);
};

#endif // EROCKTHROWERBASE_H
