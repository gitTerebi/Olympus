#include "philosopher.h"

#include "textures/egametextures.h"

Philosopher::Philosopher(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fPhilosopher,
                   eCharacterType::philosopher) {
    eGameTextures::loadPhilosopher();
    setProvide(eProvide::philosopherInventor, 10000);
}
