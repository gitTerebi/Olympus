#include "healer.h"

#include "textures/game-textures.h"

Healer::Healer(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fHealer,
                   eCharacterType::healer) {
    GameTextures::loadHealer();
    setProvide(eProvide::hygiene, 100000);
}
