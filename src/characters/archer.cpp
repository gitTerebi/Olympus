#include "archer.h"

#include "textures/game-textures.h"
#include "engine/game-board.h"
#include "numbers.h"

Archer::Archer(GameBoard& board) :
    eCharacter(board, eCharacterType::archer) {
    setAttack(Numbers::sWallArcherAttack);
}

std::shared_ptr<Texture>
Archer::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = GameTextures::characters()[id];
    const ArcherTextures* charTexs;
    if(atlantean()) {
        GameTextures::loadPoseidonTowerArcher();
        charTexs = &texs.fPoseidonTowerArcher;
    } else {
        GameTextures::loadArcher();
        charTexs = &texs.fArcher;
    }
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::lay:
    case eCharacterActionType::stand:
        return charTexs->fWalk[oid].getTexture(0);
    case eCharacterActionType::collect:
    case eCharacterActionType::fight:
        coll = &charTexs->fFight[oid];
        break;
    case eCharacterActionType::carry:
    case eCharacterActionType::walk:
        coll = &charTexs->fWalk[oid];
        break;
    case eCharacterActionType::patrol:
        coll = &charTexs->fPatrol[oid];
        break;
    case eCharacterActionType::die:
        wrap = false;
        coll = &charTexs->fDie;
        break;
    default:
        return nullptr;
    }

    return eCharacter::getTexture(coll, wrap, false);
}
