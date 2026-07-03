#include "eboatbase.h"

#include "engine/game-board.h"
#include "textures/game-textures.h"

namespace {
std::shared_ptr<Texture> boatTexture(const eCharacter& c,
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

    return boatTexture(*this, coll, wrap);
}
