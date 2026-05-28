#include "egymnast.h"

#include "textures/egametextures.h"

eGymnast::eGymnast(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fGymnast,
                   eCharacterType::gymnast) {
    eGameTextures::loadGymnast();
    setProvide(eProvide::gymnastScholar, 10000);
}
