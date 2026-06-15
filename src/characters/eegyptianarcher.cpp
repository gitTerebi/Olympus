#include "eegyptianarcher.h"

#include "textures/game-textures.h"

eEgyptianArcher::eEgyptianArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fEgyptianArcher,
                eCharacterType::egyptianArcher) {
    GameTextures::loadEgyptianArcher();
}
