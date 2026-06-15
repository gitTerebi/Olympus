#include "eurchingatherer.h"

#include "textures/game-textures.h"

eUrchinGatherer::eUrchinGatherer(GameBoard& board) :
    eResourceCollectorBase(board, eCharacterType::urchinGatherer) {
    GameTextures::loadUrchinGatherer();
}

std::shared_ptr<Texture> eUrchinGatherer::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = GameTextures::characters();
    const auto& colls = texs[id];
    const auto& charTexs = colls.fUrchinGatherer;
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fSwim[oid].getTexture(0);
    case eCharacterActionType::collect:
        coll = &charTexs.fCollect[oid];
        break;
    case eCharacterActionType::carry:
        coll = &charTexs.fCarry[oid];
        break;
    case eCharacterActionType::walk:
        coll = &charTexs.fSwim[oid];
        break;
    case eCharacterActionType::deposit:
        coll = &charTexs.fDeposit[oid];
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
