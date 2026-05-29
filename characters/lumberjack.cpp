#include "lumberjack.h"

#include "textures/egametextures.h"

Lumberjack::Lumberjack(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fLumberjack,
                       eCharacterType::lumberjack) {
    eGameTextures::loadLumberjack();
}
