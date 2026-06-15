#include "inventor.h"

#include "textures/game-textures.h"

Inventor::Inventor(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fInventor,
                   eCharacterType::inventor) {
    GameTextures::loadInventor();
    setProvide(eProvide::philosopherInventor, 10000);
}
