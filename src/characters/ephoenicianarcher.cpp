#include "ephoenicianarcher.h"

#include "textures/game-textures.h"

ePhoenicianArcher::ePhoenicianArcher(GameBoard& board) :
    eArcherBase(board, &CharacterTextures::fPhoenicianArcher,
                eCharacterType::phoenicianArcher) {
    GameTextures::loadPhoenicianArcher();
}
