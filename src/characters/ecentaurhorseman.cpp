#include "ecentaurhorseman.h"

#include "textures/game-textures.h"

eCentaurHorseman::eCentaurHorseman(GameBoard& board) :
    eHorsemanBase(board, &CharacterTextures::fCentaurHorseman,
                  eCharacterType::centaurHorseman) {
    GameTextures::loadCentaurHorseman();
}
