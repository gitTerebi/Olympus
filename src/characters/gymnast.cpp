#include "gymnast.h"

#include "textures/egametextures.h"

Gymnast::Gymnast(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fGymnast,
                   eCharacterType::gymnast) {
    eGameTextures::loadGymnast();
    setProvide(eProvide::gymnastScholar, 10000);
}
