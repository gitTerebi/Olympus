#include "butcher.h"

#include "textures/game-textures.h"

Butcher::Butcher(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fButcher,
                   eCharacterType::butcher) {
    GameTextures::loadButcher();
}
