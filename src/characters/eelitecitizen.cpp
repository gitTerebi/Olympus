#include "eelitecitizen.h"

#include "textures/game-textures.h"

eEliteCitizen::eEliteCitizen(GameBoard& board) :
    eFightingPatroler(board, &CharacterTextures::fEliteCitizen,
                      eCharacterType::eliteCitizen) {
    GameTextures::loadEliteCitizen();
    setAttack(0.5);
    setHP(200);
}
