#include "epersianarcher.h"

#include "textures/game-textures.h"

ePersianArcher::ePersianArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fPersianArcher,
                eCharacterType::persianArcher) {
    GameTextures::loadPersianArcher();
}
