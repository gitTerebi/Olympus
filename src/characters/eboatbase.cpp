#include "eboatbase.h"

#include "textures/game-textures.h"

eBoatBase::eBoatBase(
        GameBoard& board, const eCharTexs charTexs,
        const eCharacterType type) :
    eCharacter(board, type),
    mCharTexs(charTexs) {}

std::shared_ptr<Texture> eBoatBase::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = GameTextures::characters();
    const auto& colls = texs[id];
    const auto& charTexs = colls.*mCharTexs;
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fStand.getTexture(oid);
    case eCharacterActionType::carry:
    case eCharacterActionType::walk:
        coll = &charTexs.fSwim[oid];
        break;
    case eCharacterActionType::die:
        wrap = false;
        coll = &charTexs.fDie[oid];
        break;
    default:
        return nullptr;
    }

    return eCharacter::getTexture(coll, wrap, false);
}
