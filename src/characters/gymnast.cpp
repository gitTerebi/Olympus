#include "gymnast.h"

#include "textures/game-textures.h"

Gymnast::Gymnast(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fGymnast,
                   eCharacterType::gymnast) {
    GameTextures::loadGymnast();
    setProvide(eProvide::gymnastScholar, 10000);
}
