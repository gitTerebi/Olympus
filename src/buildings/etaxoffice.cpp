#include "etaxoffice.h"

#include "characters/tax-collector.h"
#include "textures/game-textures.h"

eTaxOffice::eTaxOffice(GameBoard& board, const eCityId cid) :
    ePatrolBuilding(board,
                    &BuildingTextures::fTaxOffice,
                    -0.65, -2.55,
                    &BuildingTextures::fTaxOfficeOverlay,
                    [this]() { return e::make_shared<TaxCollector>(getBoard()); },
                    eBuildingType::taxOffice, 2, 2, 8, cid) {
    GameTextures::loadTaxOffice();
}
