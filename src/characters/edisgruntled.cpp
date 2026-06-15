#include "edisgruntled.h"

#include "textures/game-textures.h"

eDisgruntled::eDisgruntled(GameBoard& board) :
    eFightingPatroler(board, &CharacterTextures::fDisgruntled,
                      eCharacterType::disgruntled) {
    GameTextures::loadDisgruntled();
    setAttack(4);
    setHP(10);
}
