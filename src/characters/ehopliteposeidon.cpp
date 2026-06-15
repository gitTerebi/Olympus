#include "ehopliteposeidon.h"

#include "textures/game-textures.h"

eHoplitePoseidon::eHoplitePoseidon(GameBoard& board) :
    eHopliteBase(board, &CharacterTextures::fHoplitePoseidon,
                 eCharacterType::hoplitePoseidon) {
    GameTextures::loadHoplitePoseidon();
}
