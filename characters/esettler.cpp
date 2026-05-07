#include "esettler.h"

#include "fileIO/esavearchive.h"
#include "fileIO/efileformat.h"
#include "textures/egametextures.h"

#include "erand.h"

eSettler::eSettler(eGameBoard& board) :
    eBasicPatroler(board,
                   eRand::rand() % 2 ? &eCharacterTextures::fSettlers1 :
                                &eCharacterTextures::fSettlers2,
                   eCharacterType::settler) {

    eGameTextures::loadSettlers();
}

void eSettler::setEmigrant(const bool e) {
    mEmigrant = e;
}

void eSettler::serialize(eSaveArchive& ar) {
    ar.valueSince(eFileFormat::settlerEmigrant, mEmigrant, false);
}

void eSettler::read(eReadStream& src) {
    eBasicPatroler::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eSettler::write(eWriteStream& dst) const {
    eBasicPatroler::write(dst);
    eSaveArchive ar(dst);
    const_cast<eSettler*>(this)->serialize(ar);
}
