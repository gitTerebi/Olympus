#include "esettler.h"

#include "fileIO/esavearchive.h"
#include "textures/egametextures.h"

#include "erand.h"

eSettler::eSettler(GameBoard& board) :
    eBasicPatroler(board,
                   eRand::rand() % 2 ? &eCharacterTextures::fSettlers1 :
                                &eCharacterTextures::fSettlers2,
                   eCharacterType::settler) {

    eGameTextures::loadSettlers();
}

void eSettler::setEmigrant(const bool e) {
    mEmigrant = e;
}

void eSettler::serializeFields(eSaveArchive& ar) {
    eBasicPatroler::serializeFields(ar);
    ar.field("mEmigrant", mEmigrant);
}
