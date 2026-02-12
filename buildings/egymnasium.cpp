#include "egymnasium.h"

#include "characters/egymnast.h"
#include "textures/egametextures.h"

eGymnasium::eGymnasium(eGameBoard& board, const eCityId cid) :
    ePatrolSourceBuilding(board,
                    &eBuildingTextures::fGymnasium,
                    -2.63, -4.55,
                    &eBuildingTextures::fGymnasiumOverlay,
                    {{eCharacterType::competitor, eBuildingType::stadium}},
                    eBuildingType::gymnasium, 3, 3, 7, cid,
                    [this]() { return e::make_shared<eGymnast>(getBoard()); }) {
    eGameTextures::loadGymnasium();
}
