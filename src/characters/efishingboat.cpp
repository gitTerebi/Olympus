#include "efishingboat.h"

#include "textures/game-textures.h"

eFishingBoat::eFishingBoat(GameBoard& board) :
    eResourceCollectorBase(board, eCharacterType::fishingBoat) {
    GameTextures::loadFishingBoat();
}

std::shared_ptr<Texture> eFishingBoat::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = GameTextures::characters();
    const auto& colls = texs[id];
    const auto& charTexs = colls.fFishingBoat;
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fStand.getTexture(oid);
    case eCharacterActionType::collect:
        coll = &charTexs.fCollect[oid];
        break;
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
