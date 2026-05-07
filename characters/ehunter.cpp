#include "ehunter.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"

eHunter::eHunter(eGameBoard& board) :
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

void eHunter::read(eReadStream& src) {
    eResourceCollector::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eHunter::write(eWriteStream& dst) const {
    eResourceCollector::write(dst);
    eSaveArchive ar(dst);
    const_cast<eHunter*>(this)->serialize(ar);
}

void eHunter::serialize(eSaveArchive& ar) {
    bool deerHunter = mDeerHunter;
    ar.value(deerHunter);
    if(ar.reading()) setDeerHunter(deerHunter);
}

