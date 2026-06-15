#include "tax-collector.h"

#include "textures/game-textures.h"

#include "elimits.h"

TaxCollector::TaxCollector(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fTaxCollector,
                   eCharacterType::taxCollector) {
    GameTextures::loadTaxCollector();
    setProvide(eProvide::taxes, __INT_MAX__);
}
