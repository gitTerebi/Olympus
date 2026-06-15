#include "emayanarcher.h"

#include "textures/game-textures.h"

eMayanArcher::eMayanArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fMayanArcher,
                eCharacterType::mayanArcher) {
    GameTextures::loadMayanArcher();
}
