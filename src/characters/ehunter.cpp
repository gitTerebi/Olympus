#include "ehunter.h"

#include "textures/game-textures.h"
#include "fileIO/save-archive.h"

eHunter::eHunter(GameBoard& board) :
    eResourceCollector(board, &CharacterTextures::fHunter,
                       eCharacterType::hunter) {
    GameTextures::loadHunter();
    setAttack(0.3);
}

void eHunter::setDeerHunter(const bool h) {
    mDeerHunter = h;
    if(h) {
        setCharTexs(&CharacterTextures::fDeerHunter);
    } else {
        setCharTexs(&CharacterTextures::fHunter);
    }
}

void eHunter::serializeFields(SaveArchive& ar) {
    eResourceCollector::serializeFields(ar);
    bool deerHunter = mDeerHunter;
    ar.field("deerHunter", deerHunter);
    if(ar.reading()) setDeerHunter(deerHunter);
}

