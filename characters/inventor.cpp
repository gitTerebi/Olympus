#include "inventor.h"

#include "textures/egametextures.h"

Inventor::Inventor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fInventor,
                   eCharacterType::inventor) {
    eGameTextures::loadInventor();
    setProvide(eProvide::philosopherInventor, 10000);
}
