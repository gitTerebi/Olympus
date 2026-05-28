#include "elumberjack.h"

#include "textures/egametextures.h"

eLumberjack::eLumberjack(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fLumberjack,
                       eCharacterType::lumberjack) {
    eGameTextures::loadLumberjack();
}
