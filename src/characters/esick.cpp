#include "esick.h"

#include "textures/game-textures.h"

eSick::eSick(GameBoard& board) :
    eFightingPatroler(board, &CharacterTextures::fSick,
                      eCharacterType::sick) {
    GameTextures::loadSick();
    setSpeed(18.375);
}
