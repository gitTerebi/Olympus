#include "esettler.h"

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

void eSettler::read(eReadStream& src) {
    eBasicPatroler::read(src);
    const auto ver = src.formatVersion();
    if(ver >= eFileFormat::settlerEmigrant) {
        src >> mEmigrant;
    }
}

void eSettler::write(eWriteStream& dst) const {
    eBasicPatroler::write(dst);
    dst << mEmigrant;
}
