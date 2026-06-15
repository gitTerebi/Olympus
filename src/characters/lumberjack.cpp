#include "lumberjack.h"

#include "textures/game-textures.h"

Lumberjack::Lumberjack(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fLumberjack,
                       eCharacterType::lumberjack) {
    GameTextures::loadLumberjack();
}
