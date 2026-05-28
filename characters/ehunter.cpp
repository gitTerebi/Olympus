#include "ehunter.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"

eHunter::eHunter(GameBoard& board) :
    eResourceCollector(board, &eCharacterTextures::fHunter,
                       eCharacterType::hunter) {
    eGameTextures::loadHunter();
    setAttack(0.3);
}

void eHunter::setDeerHunter(const bool h) {
    mDeerHunter = h;
    if(h) {
        setCharTexs(&eCharacterTextures::fDeerHunter);
    } else {
        setCharTexs(&eCharacterTextures::fHunter);
    }
}

void eHunter::serializeFields(eSaveArchive& ar) {
    eResourceCollector::serializeFields(ar);
    bool deerHunter = mDeerHunter;
    ar.field("deerHunter", deerHunter);
    if(ar.reading()) setDeerHunter(deerHunter);
}

