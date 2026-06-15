#include "god-missile.h"
#include "fileIO/save-archive.h"

#include "textures/game-textures.h"
#include "characters/heroes/ehero.h"

GodMissile::GodMissile(GameBoard& board,
                         const std::vector<PathPoint>& path) :
    Missile(board, MissileType::god, path) {}

std::shared_ptr<Texture>
GodMissile::getTexture(const eTileSize size) const {
    auto& board = this->board();
    const auto dir = board.direction();
    using eTexPtr = std::vector<TextureCollection> DestructionTextures::*;
    eTexPtr collsptr;
    if(mActionType == eCharacterActionType::bless) {
        GameTextures::loadBless();
        collsptr = &DestructionTextures::fBless;
    } else if(mActionType == eCharacterActionType::curse) {
        GameTextures::loadCurse();
        collsptr = &DestructionTextures::fCurse;
    } else {
        switch(mCharType) {
        case eCharacterType::aphrodite:
        case eCharacterType::apollo:
        case eCharacterType::ares:
        case eCharacterType::artemis:
        case eCharacterType::athena:
        case eCharacterType::atlas:
        case eCharacterType::demeter:
        case eCharacterType::dionysus:
        case eCharacterType::hades:
        case eCharacterType::hephaestus:
        case eCharacterType::hera:
        case eCharacterType::hermes:
        case eCharacterType::poseidon:
        case eCharacterType::zeus: {
            const auto gt = God::sCharacterToGodType(mCharType);
            collsptr = God::sGodMissile(gt);
        } break;
        case eCharacterType::atalanta: {
            const auto ht = eHero::sCharacterToHeroType(mCharType);
            collsptr = eHero::sHeroMissile(ht);
        } break;
        case eCharacterType::hydra:
            GameTextures::loadGodBlueArrow();
            collsptr = &DestructionTextures::fGodBlueArrow;
            break;
        default:
            GameTextures::loadMonsterMissile();
            collsptr = &DestructionTextures::fMonsterMissile;
        }
    }

    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const double a = angle();
    const auto o = sAngleOrientation(a);
    const auto oo = sRotated(o, dir);
    const int ooid = static_cast<int>(oo);
    const auto& colls = textures[id].*collsptr;
    const auto& coll = colls[ooid];
    const int texId = textureTime();
    return coll.getTexture(texId % coll.size());
}

void GodMissile::setTexture(const eCharacterType ct,
                             const eCharacterActionType cat) {
    mCharType = ct;
    mActionType = cat;
}

void GodMissile::serializeFields(SaveArchive& ar) {
    Missile::serializeFields(ar);
    ar.field("mCharType", mCharType);
    ar.field("mActionType", mActionType);
}
