#include "epeddler.h"

#include "buildings/eagorabase.h"
#include "textures/echaractertextures.h"
#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"
#include "engine/e-game-board.h"

ePeddler::ePeddler(eGameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fPeddler,
                   eCharacterType::peddler) {
    setProvide(eProvide::peddler, 10000);
    eGameTextures::loadPeddler();
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

void ePeddler::serializeFields(eSaveArchive& ar) {
    eBasicPatroler::serializeFields(ar);
    ar.buildingAsField("agora", &getBoard(), mAgora);
}
