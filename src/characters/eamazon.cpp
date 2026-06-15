#include "eamazon.h"

#include "textures/game-textures.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eAmazon::eAmazon(GameBoard& board) :
    eSoldier(board, nullptr, eCharacterType::amazon) {
    setIsArcher(false);
}

void eAmazon::setIsArcher(const bool a) {
    mIsArcher = a;
    if(a) {
        GameTextures::loadAmazonArcher();
        setCharTexs(&CharacterTextures::fAmazonArcher);
        setRange(eNumbers::sArcherRange);
        setAttack(eNumbers::sArcherAttack);
        setHP(eNumbers::sArcherHP);
    } else {
        GameTextures::loadAmazonSpear();
        setCharTexs(&CharacterTextures::fAmazonSpear);
        setRange(0);
        setAttack(eNumbers::sHopliteAttack);
        setHP(eNumbers::sHopliteHP);
    }
}

void eAmazon::serializeFields(eSaveArchive& ar) {
    eSoldier::serializeFields(ar);
    bool archer = mIsArcher;
    ar.field("archer", archer);
    if(ar.reading()) setIsArcher(archer);
}

