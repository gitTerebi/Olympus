#include "eoceanidspearthrower.h"

#include "textures/game-textures.h"

eOceanidSpearthrower::eOceanidSpearthrower(GameBoard& board) :
    eSpearthrowerBase(board, &CharacterTextures::fOceanidSpearthrower,
                      eCharacterType::oceanidSpearthrower) {
    GameTextures::loadOceanidSpearthrower();
}
