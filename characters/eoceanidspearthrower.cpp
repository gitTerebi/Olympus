#include "eoceanidspearthrower.h"

#include "textures/egametextures.h"

eOceanidSpearthrower::eOceanidSpearthrower(GameBoard& board) :
    eSpearthrowerBase(board, &eCharacterTextures::fOceanidSpearthrower,
                      eCharacterType::oceanidSpearthrower) {
    eGameTextures::loadOceanidSpearthrower();
}
