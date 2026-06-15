#include "scholar.h"

#include "textures/game-textures.h"

Scholar::Scholar(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fScholar,
                   eCharacterType::scholar) {
    GameTextures::loadScholar();
    setProvide(eProvide::gymnastScholar, 10000);
}
