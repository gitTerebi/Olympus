#include "priest.h"

#include "textures/character-textures.h"
#include "textures/game-textures.h"

Priest::Priest(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fPriest,
                   eCharacterType::priest) {
    GameTextures::loadPriest();
}
