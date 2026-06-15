#include "eareswarrior.h"

#include "textures/character-textures.h"
#include "textures/game-textures.h"
#include "enumbers.h"

eAresWarrior::eAresWarrior(GameBoard& board) :
    eSoldier(board, nullptr, eCharacterType::aresWarrior) {
    GameTextures::loadAresWarrior();
    setAttack(eNumbers::sHopliteAttack);
    setHP(eNumbers::sHopliteHP);
}

std::shared_ptr<eTexture>
eAresWarrior::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& gTexs = GameTextures::characters();
    const auto& texs = gTexs[id].fAresWarrior;
    const eTextureCollection* coll = nullptr;
    bool wrap = true;
    const int oid = static_cast<int>(rotatedOrientation());
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return texs.fWalk[oid].getTexture(0);
    case eCharacterActionType::collect:
    case eCharacterActionType::fight:
    case eCharacterActionType::fight2:
    case eCharacterActionType::bless:
    case eCharacterActionType::curse: {
        wrap = true;
        coll = &texs.fFight[oid];
    } break;
    case eCharacterActionType::carry:
    case eCharacterActionType::walk: {
        coll = &texs.fWalk[oid];
    } break;
    case eCharacterActionType::disappear:
    case eCharacterActionType::die:
        wrap = false;
        coll = &texs.fDie[oid];
        break;
    default:
        return nullptr;
    }

    return eCharacter::getTexture(coll, wrap, false);
}
