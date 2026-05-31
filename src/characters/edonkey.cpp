#include "edonkey.h"

#include "textures/egametextures.h"

eDonkey::eDonkey(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fDonkey,
                   eCharacterType::donkey) {
    eGameTextures::loadDonkey();
}
