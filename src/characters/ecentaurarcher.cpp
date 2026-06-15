#include "ecentaurarcher.h"

#include "textures/game-textures.h"

eCentaurArcher::eCentaurArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fCentaurArcher,
                eCharacterType::centaurArcher) {
    GameTextures::loadCentaurArcher();
    setSpeed(78.75);
}
