#include "esettler.h"

#include "fileIO/save-archive.h"
#include "textures/game-textures.h"

#include "rand.h"

eSettler::eSettler(GameBoard& board) :
    eBasicPatroler(board,
                   Rand::rand() % 2 ? &CharacterTextures::fSettlers1 :
                                &CharacterTextures::fSettlers2,
                   eCharacterType::settler) {

    GameTextures::loadSettlers();
}

void eSettler::setEmigrant(const bool e) {
    mEmigrant = e;
}

void eSettler::serializeFields(SaveArchive& ar) {
    eBasicPatroler::serializeFields(ar);
    ar.field("mEmigrant", mEmigrant);
}
