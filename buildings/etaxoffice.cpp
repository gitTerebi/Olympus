#include "etaxoffice.h"

#include "characters/tax-collector.h"
#include "textures/egametextures.h"

eTaxOffice::eTaxOffice(GameBoard& board, const eCityId cid) :
    ePatrolBuilding(board,
                    &eBuildingTextures::fTaxOffice,
                    -0.65, -2.55,
                    &eBuildingTextures::fTaxOfficeOverlay,
                    [this]() { return e::make_shared<TaxCollector>(getBoard()); },
                    eBuildingType::taxOffice, 2, 2, 8, cid) {
    eGameTextures::loadTaxOffice();
}
