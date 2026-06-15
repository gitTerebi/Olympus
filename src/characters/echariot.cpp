#include "echariot.h"

#include "textures/game-textures.h"

eChariot::eChariot(GameBoard& board) :
    eCharacter(board, eCharacterType::chariot) {
    GameTextures::loadChariot();
}

std::shared_ptr<Texture>
eChariot::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& gTexs = GameTextures::characters();
    const auto& charTexs = gTexs[id].fChariot;
    const int oid = static_cast<int>(rotatedOrientation());
    const auto& coll = &charTexs[oid];
    return eCharacter::getTexture(coll, true, false);
}
