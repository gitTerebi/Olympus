#include "edairy.h"

#include "characters/goatherd.h"
#include "textures/game-textures.h"
#include "characters/actions/shepherd-action.h"

#include <algorithm>

stdsptr<eResourceCollectorBase> dairyCharGenerator(GameBoard& board) {
    return e::make_shared<Goatherd>(board);
}

eDairy::eDairy(GameBoard& board, const eCityId cid) :
    eShepherBuildingBase(board, &BuildingTextures::fDairy,
                         -1.35, -2.95,
                         &BuildingTextures::fDairyOverlay,
                         dairyCharGenerator,
                         eBuildingType::dairy,
                         eResourceType::cheese,
                         eCharacterType::goat,
                         2, 2, 8, cid),
    mTextures(GameTextures::buildings())  {
    GameTextures::loadDairy();
}

std::vector<eOverlay> eDairy::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = mTextures[sizeId];
    auto os = eShepherBuildingBase::getOverlays(size);
    if(resource() > 0) {
        eOverlay cheeese;
        const int res = std::clamp(resource() - 1, 0, 4);
        cheeese.fTex = texs.fWaitingCheese.getTexture(res);
        cheeese.fX = 0;
        cheeese.fY = -1.5;

        os.push_back(cheeese);
    }
    return os;
}
