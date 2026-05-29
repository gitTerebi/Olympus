#include "tax-collector.h"

#include "textures/egametextures.h"

#include "elimits.h"

TaxCollector::TaxCollector(GameBoard& board) :
    eBasicPatroler(board, &eCharacterTextures::fTaxCollector,
                   eCharacterType::taxCollector) {
    eGameTextures::loadTaxCollector();
    setProvide(eProvide::taxes, __INT_MAX__);
}
