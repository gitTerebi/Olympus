#include "egodmissile.h"

#include "textures/egametextures.h"
#include "characters/heroes/ehero.h"

eGodMissile::eGodMissile(eGameBoard& board,
                         const std::vector<ePathPoint>& path) :
    eMissile(board, eMissileType::god, path) {}

std::shared_ptr<eTexture>
eGodMissile::getTexture(const eTileSize size) const {
    auto& board = this->board();
    const auto dir = board.direction();
    using eTexPtr = std::vector<eTextureCollection> eDestructionTextures::*;
    eTexPtr collsptr;
    if(mActionType == eCharacterActionType::bless) {
        eGameTextures::loadBless();
        collsptr = &eDestructionTextures::fBless;
    } else if(mActionType == eCharacterActionType::curse) {
        eGameTextures::loadCurse();
        collsptr = &eDestructionTextures::fCurse;
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
            const auto gt = eGod::sCharacterToGodType(mCharType);
            collsptr = eGod::sGodMissile(gt);
        } break;
        case eCharacterType::atalanta: {
            const auto ht = eHero::sCharacterToHeroType(mCharType);
            collsptr = eHero::sHeroMissile(ht);
        } break;
        default:
            eGameTextures::loadMonsterMissile();
            collsptr = &eDestructionTextures::fMonsterMissile;
        }
    }

    const int id = static_cast<int>(size);
    const auto& textures = eGameTextures::destrution();
    const double a = angle();
    const auto o = sAngleOrientation(a);
    const auto oo = sRotated(o, dir);
    const int ooid = static_cast<int>(oo);
    const auto& colls = textures[id].*collsptr;
    const auto& coll = colls[ooid];
    const int texId = textureTime();
    return coll.getTexture(texId % coll.size());
}

void eGodMissile::setTexture(const eCharacterType ct,
                             const eCharacterActionType cat) {
    mCharType = ct;
    mActionType = cat;
}

void eGodMissile::read(eReadStream& src) {
    eMissile::read(src);
    src >> mCharType;
    src >> mActionType;
}

void eGodMissile::write(eWriteStream& dst) const {
    eMissile::write(dst);
    dst << mCharType;
    dst << mActionType;
}
