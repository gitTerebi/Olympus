#include "actor.h"

#include "textures/egametextures.h"

Actor::Actor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fActor,
                   eCharacterType::actor) {
    setProvide(eProvide::actorAstronomer, 10000);
    eGameTextures::loadActor();
}
