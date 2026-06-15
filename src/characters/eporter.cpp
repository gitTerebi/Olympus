#include "eporter.h"

#include "textures/game-textures.h"

ePorter::ePorter(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fPorter,
                   eCharacterType::porter) {
    GameTextures::loadPorter();
}
