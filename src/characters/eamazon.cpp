#include "eamazon.h"

#include "textures/game-textures.h"
#include "numbers.h"
#include "fileIO/save-archive.h"

eAmazon::eAmazon(GameBoard& board) :
    eSoldier(board, nullptr, eCharacterType::amazon) {
    setIsArcher(false);
}

void eAmazon::setIsArcher(const bool a) {
    mIsArcher = a;
    if(a) {
        GameTextures::loadAmazonArcher();
        setCharTexs(&CharacterTextures::fAmazonArcher);
        setRange(Numbers::sArcherRange);
        setAttack(Numbers::sArcherAttack);
        setHP(Numbers::sArcherHP);
    } else {
        GameTextures::loadAmazonSpear();
        setCharTexs(&CharacterTextures::fAmazonSpear);
        setRange(0);
        setAttack(Numbers::sHopliteAttack);
        setHP(Numbers::sHopliteHP);
    }
}

void eAmazon::serializeFields(SaveArchive& ar) {
    eSoldier::serializeFields(ar);
    bool archer = mIsArcher;
    ar.field("archer", archer);
    if(ar.reading()) setIsArcher(archer);
}

