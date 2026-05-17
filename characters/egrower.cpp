#include "egrower.h"

#include "fileIO/esavearchive.h"
#include "textures/egametextures.h"

eGrower::eGrower(eGameBoard& board) :
    eCharacter(board, eCharacterType::grower) {
    eGameTextures::loadGrower();
}

void eGrower::incGrapes(const int i) {
    mGrapes += i;
}

void eGrower::incOlives(const int i) {
    mOlives += i;
}

void eGrower::incOranges(const int i) {
    mOranges += i;
}

void eGrower::read(eReadStream& src) {
    eCharacter::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eGrower::write(eWriteStream& dst) const {
    eCharacter::write(dst);
    eSaveArchive ar(dst);
    const_cast<eGrower*>(this)->serialize(ar);
}

void eGrower::serialize(eSaveArchive& ar) {
    ar.field("growerType", mType, eGrowerType::grapesAndOlives);
    ar.field("grapesCount", mGrapes, 0);
    ar.field("olivesCount", mOlives, 0);
    ar.field("orangesCount", mOranges, 0);
}

std::shared_ptr<eTexture> eGrower::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = eGameTextures::characters();
    const auto& colls = texs[id];
    switch(mType) {
    case eGrowerType::grapesAndOlives:
        return getGrapesAndOlivesTex(colls);
    case eGrowerType::oranges:
        return getOrangesTex(colls);
    }
    return nullptr;
}

std::shared_ptr<eTexture> eGrower::getGrapesAndOlivesTex(
        const eCharacterTextures& texs) const {
    const auto& charTexs = texs.fGrower;
    const eTextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fWalk[oid].getTexture(0);
    case eCharacterActionType::workOnGrapes:
        coll = &charTexs.fWorkOnGrapes[oid];
        break;
    case eCharacterActionType::fight:
    case eCharacterActionType::workOnOlives:
        coll = &charTexs.fWorkOnOlives[oid];
        break;
    case eCharacterActionType::collectGrapes:
        coll = &charTexs.fCollectGrapes[oid];
        break;
    case eCharacterActionType::collectOlives:
        coll = &charTexs.fCollectOlives[oid];
        break;
    case eCharacterActionType::walk:
        coll = &charTexs.fWalk[oid];
        break;
    case eCharacterActionType::carry:
        coll = &charTexs.fWalk[oid];
        break;
    case eCharacterActionType::die:
        wrap = false;
        coll = &charTexs.fDie;
        break;
    default: return std::shared_ptr<eTexture>();
    }

    return eCharacter::getTexture(coll, wrap, false);
}

std::shared_ptr<eTexture> eGrower::getOrangesTex(
        const eCharacterTextures& texs) const {
    const auto& charTexs = texs.fOrangeTender;
    const eTextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::stand:
        return charTexs.fWalk[oid].getTexture(0);
    case eCharacterActionType::fight:
    case eCharacterActionType::workOnOranges:
        coll = &charTexs.fWorkOnTree[oid];
        break;
    case eCharacterActionType::collectOranges:
        coll = &charTexs.fCollect[oid];
        break;
    case eCharacterActionType::walk:
        coll = &charTexs.fWalk[oid];
        break;
    case eCharacterActionType::carry:
        coll = &charTexs.fWalk[oid];
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
