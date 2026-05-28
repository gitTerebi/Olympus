#include "erockthrower.h"

#include "textures/egametextures.h"

eRockThrower::eRockThrower(GameBoard& board) :
    eRockThrowerBase(board, &eCharacterTextures::fRockThrower,
                     eCharacterType::rockThrower) {
    eGameTextures::loadRockThrower();
}
