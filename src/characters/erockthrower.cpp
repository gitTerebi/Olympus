#include "erockthrower.h"

#include "textures/game-textures.h"

eRockThrower::eRockThrower(GameBoard& board) :
    eRockThrowerBase(board, &CharacterTextures::fRockThrower,
                     eCharacterType::rockThrower) {
    GameTextures::loadRockThrower();
}
