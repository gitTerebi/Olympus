#include "edonkey.h"

#include "textures/game-textures.h"

eDonkey::eDonkey(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fDonkey,
                   eCharacterType::donkey) {
    GameTextures::loadDonkey();
}
