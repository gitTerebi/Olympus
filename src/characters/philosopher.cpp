#include "philosopher.h"

#include "textures/game-textures.h"

Philosopher::Philosopher(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fPhilosopher,
                   eCharacterType::philosopher) {
    GameTextures::loadPhilosopher();
    setProvide(eProvide::philosopherInventor, 10000);
}
