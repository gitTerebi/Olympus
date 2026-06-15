#include "etrojanspearthrower.h"

#include "textures/game-textures.h"

eTrojanSpearthrower::eTrojanSpearthrower(GameBoard& board) :
    eSpearthrowerBase(board, &CharacterTextures::fTrojanSpearthrower,
                      eCharacterType::trojanSpearthrower) {
    GameTextures::loadTrojanSpearthrower();
}
