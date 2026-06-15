#include "actor.h"

#include "textures/game-textures.h"

Actor::Actor(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fActor,
                   eCharacterType::actor) {
    setProvide(eProvide::actorAstronomer, 10000);
    GameTextures::loadActor();
}
