#include "etaxcollector.h"

#include "textures/egametextures.h"

#include "elimits.h"

eTaxCollector::eTaxCollector(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fTaxCollector,
                   eCharacterType::taxCollector) {
    eGameTextures::loadTaxCollector();
    setProvide(eProvide::taxes, __INT_MAX__);
}
