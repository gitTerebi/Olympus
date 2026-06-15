#include "egymnasium.h"

#include "characters/gymnast.h"
#include "textures/game-textures.h"

eGymnasium::eGymnasium(GameBoard& board, const eCityId cid) :
    ePatrolSourceBuilding(board,
                    &BuildingTextures::fGymnasium,
                    -2.63, -4.55,
                    &BuildingTextures::fGymnasiumOverlay,
                    {{eCharacterType::competitor, eBuildingType::stadium}},
                    eBuildingType::gymnasium, 3, 3, 7, cid,
                    [this]() { return e::make_shared<Gymnast>(getBoard()); }) {
    GameTextures::loadGymnasium();
}
