#include "eox.h"

#include "textures/game-textures.h"

eOx::eOx(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fOx,
                   eCharacterType::ox) {
    GameTextures::loadOx();
}
