#include "watchman.h"

#include "textures/game-textures.h"
#include "numbers.h"

Watchman::Watchman(GameBoard& board) :
    eFightingPatroler(board, &CharacterTextures::fWatchman,
                      eCharacterType::watchman) {
    GameTextures::loadWatchman();
    setProvide(eProvide::satisfaction, 100000);
    setAttack(Numbers::sWatchmanAttack);
    setHP(Numbers::sWatchmanHP);
}
