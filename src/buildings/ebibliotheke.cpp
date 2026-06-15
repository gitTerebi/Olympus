#include "ebibliotheke.h"

#include "characters/scholar.h"
#include "textures/game-textures.h"

eBibliotheke::eBibliotheke(GameBoard& board,
                           const eCityId cid) :
    ePatrolBuilding(board,
                    &BuildingTextures::fBibliotheke,
                    -1.12, -3.17,
                    &BuildingTextures::fBibliothekeOverlay,
                    [this]() { return e::make_shared<Scholar>(getBoard()); },
                    eBuildingType::bibliotheke, 2, 2, 5, cid) {
    GameTextures::loadBibliotheke();
}
