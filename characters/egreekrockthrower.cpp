#include "egreekrockthrower.h"

#include "textures/egametextures.h"

eGreekRockThrower::eGreekRockThrower(GameBoard& board) :
    eRockThrowerBase(board, &eCharacterTextures::fGreekRockThrower,
                     eCharacterType::greekRockThrower) {
    eGameTextures::loadGreekRockThrower();
}
