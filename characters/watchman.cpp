#include "watchman.h"

#include "textures/egametextures.h"
#include "enumbers.h"

Watchman::Watchman(GameBoard& board) :
    eFightingPatroler(board, &eCharacterTextures::fWatchman,
                      eCharacterType::watchman) {
    eGameTextures::loadWatchman();
    setProvide(eProvide::satisfaction, 100000);
    setAttack(eNumbers::sWatchmanAttack);
    setHP(eNumbers::sWatchmanHP);
}
