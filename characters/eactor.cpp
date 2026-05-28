#include "eactor.h"

#include "textures/egametextures.h"

eActor::eActor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fActor,
                   eCharacterType::actor) {
    setProvide(eProvide::actorAstronomer, 10000);
    eGameTextures::loadActor();
}
