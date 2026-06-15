#include "erangesoldier.h"

#include "textures/game-textures.h"

eRangeSoldier::eRangeSoldier(GameBoard& board,
                             const eCharTexs charTexs,
                             const eCharacterType type,
                             const int range) :
    eSoldier(board, &CharacterTextures::fHoplite, type),
    mCharTexs(charTexs) {
    setRange(range);
}

std::shared_ptr<Texture> eRangeSoldier::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::characters();
    const auto& charTexs = textures[id].*mCharTexs;
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fWalk[oid].getTexture(0);
    case eCharacterActionType::collect:
    case eCharacterActionType::carry:
    case eCharacterActionType::walk:
        coll = &charTexs.fWalk[oid];
        break;
    case eCharacterActionType::fight:
        coll = &charTexs.fFight[oid];
        break;
    case eCharacterActionType::fight2:
        coll = &charTexs.fFight2[oid];
        break;
    case eCharacterActionType::die:
        wrap = false;
        coll = &charTexs.fDie;
        break;
    default:
        return nullptr;
    }

    return eCharacter::getTexture(coll, wrap, false);
}
