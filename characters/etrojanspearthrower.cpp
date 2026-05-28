#include "etrojanspearthrower.h"

#include "textures/egametextures.h"

eTrojanSpearthrower::eTrojanSpearthrower(GameBoard& board) :
    eSpearthrowerBase(board, &eCharacterTextures::fTrojanSpearthrower,
                      eCharacterType::trojanSpearthrower) {
    eGameTextures::loadTrojanSpearthrower();
}
