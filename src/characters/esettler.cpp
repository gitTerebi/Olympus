#include "esettler.h"

#include "fileIO/esavearchive.h"
#include "textures/game-textures.h"

#include "erand.h"

eSettler::eSettler(GameBoard& board) :
    eBasicPatroler(board,
                   eRand::rand() % 2 ? &CharacterTextures::fSettlers1 :
                                &CharacterTextures::fSettlers2,
                   eCharacterType::settler) {

    GameTextures::loadSettlers();
}

void eSettler::setEmigrant(const bool e) {
    mEmigrant = e;
}

void eSettler::serializeFields(eSaveArchive& ar) {
    eBasicPatroler::serializeFields(ar);
    ar.field("mEmigrant", mEmigrant);
}
