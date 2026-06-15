#include "eegyptianchariot.h"

#include "textures/character-textures.h"
#include "textures/game-textures.h"

eEgyptianChariot::eEgyptianChariot(GameBoard& board) :
    eChariotBase(board, &CharacterTextures::fEgyptianChariot,
                 eCharacterType::egyptianChariot) {
    GameTextures::loadEgyptianChariot();
}
