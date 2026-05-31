#include "ephoenicianarcher.h"

#include "textures/egametextures.h"

ePhoenicianArcher::ePhoenicianArcher(GameBoard& board) :
    eArcherBase(board, &eCharacterTextures::fPhoenicianArcher,
                eCharacterType::phoenicianArcher) {
    eGameTextures::loadPhoenicianArcher();
}
