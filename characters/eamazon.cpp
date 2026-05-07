#include "eamazon.h"

#include "textures/egametextures.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eAmazon::eAmazon(eGameBoard& board) :
    eSoldier(board, nullptr, eCharacterType::amazon) {
    setIsArcher(false);
}

void eAmazon::setIsArcher(const bool a) {
    mIsArcher = a;
    if(a) {
        eGameTextures::loadAmazonArcher();
        setCharTexs(&eCharacterTextures::fAmazonArcher);
        setRange(eNumbers::sArcherRange);
        setAttack(eNumbers::sArcherAttack);
        setHP(eNumbers::sArcherHP);
    } else {
        eGameTextures::loadAmazonSpear();
        setCharTexs(&eCharacterTextures::fAmazonSpear);
        setRange(0);
        setAttack(eNumbers::sHopliteAttack);
        setHP(eNumbers::sHopliteHP);
    }
}

void eAmazon::read(eReadStream& src) {
    eSoldier::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eAmazon::write(eWriteStream& dst) const {
    eSoldier::write(dst);
    eSaveArchive ar(dst);
    const_cast<eAmazon*>(this)->serialize(ar);
}

void eAmazon::serialize(eSaveArchive& ar) {
    bool archer = mIsArcher;
    ar.field("archer", archer);
    if(ar.reading()) setIsArcher(archer);
}

