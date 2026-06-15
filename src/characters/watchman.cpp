#include "watchman.h"

#include "textures/game-textures.h"
#include "enumbers.h"

Watchman::Watchman(GameBoard& board) :
    eFightingPatroler(board, &CharacterTextures::fWatchman,
                      eCharacterType::watchman) {
    GameTextures::loadWatchman();
    setProvide(eProvide::satisfaction, 100000);
    setAttack(eNumbers::sWatchmanAttack);
    setHP(eNumbers::sWatchmanHP);
}
