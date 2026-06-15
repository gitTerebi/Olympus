#include "epeddler.h"

#include "buildings/eagorabase.h"
#include "textures/character-textures.h"
#include "textures/game-textures.h"
#include "fileIO/save-archive.h"
#include "engine/game-board.h"

ePeddler::ePeddler(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fPeddler,
                   eCharacterType::peddler) {
    setProvide(eProvide::peddler, 10000);
    GameTextures::loadPeddler();
}

void ePeddler::provideToBuilding(eBuilding* const b) {
    if(!mAgora) return;
    mAgora->agoraProvide(b);
}

void ePeddler::setAgora(eAgoraBase* const a) {
    mAgora = a;
}

eAgoraBase *ePeddler::agora() const {
    return mAgora.get();
}

void ePeddler::serializeFields(SaveArchive& ar) {
    eBasicPatroler::serializeFields(ar);
    ar.buildingAsField("agora", &getBoard(), mAgora);
}
