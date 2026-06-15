#include "egreekrockthrower.h"

#include "textures/game-textures.h"

eGreekRockThrower::eGreekRockThrower(GameBoard& board) :
    eRockThrowerBase(board, &CharacterTextures::fGreekRockThrower,
                     eCharacterType::greekRockThrower) {
    GameTextures::loadGreekRockThrower();
}
