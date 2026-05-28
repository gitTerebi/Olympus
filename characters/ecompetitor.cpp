#include "ecompetitor.h"

#include "textures/egametextures.h"

eCompetitor::eCompetitor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fCompetitor,
                   eCharacterType::competitor) {
    eGameTextures::loadCompetitor();
    setProvide(eProvide::competitorCurator, 10000);
}
