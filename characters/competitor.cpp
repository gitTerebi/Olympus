#include "competitor.h"

#include "textures/egametextures.h"

Competitor::Competitor(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fCompetitor,
                   eCharacterType::competitor) {
    eGameTextures::loadCompetitor();
    setProvide(eProvide::competitorCurator, 10000);
}
