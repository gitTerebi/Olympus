#include "efishingboat.h"

#include "engine/game-board.h"
#include "textures/game-textures.h"

namespace {
std::shared_ptr<Texture> fishingBoatTexture(
        const eFishingBoat& c,
        const TextureCollection* const coll,
        const bool wrap) {
    if(!coll) return nullptr;
    const int s = coll->size();
    if(s == 0) return nullptr;

    int t = c.getBoard().frame();
    if(!wrap) t = std::clamp(t, 0, s - 1);
    const int texId = ((t % s) + s) % s;
    return coll->getTexture(texId);
}
}

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

    return fishingBoatTexture(*this, coll, wrap);
}
