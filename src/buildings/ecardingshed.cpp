#include "ecardingshed.h"

#include "characters/shepherd.h"
#include "textures/game-textures.h"
#include "characters/actions/shepherd-action.h"

#include <algorithm>

stdsptr<eResourceCollectorBase> cardingShedCharGenerator(GameBoard& board) {
    return e::make_shared<Shepherd>(board);
}

eCardingShed::eCardingShed(GameBoard& board, const eCityId cid) :
    eShepherBuildingBase(board, &BuildingTextures::fCardingShed,
                         -0.98, -2.15,
                         &BuildingTextures::fCardingShedOverlay,
                         cardingShedCharGenerator,
                         eBuildingType::cardingShed,
                         eResourceType::fleece,
                         eCharacterType::sheep,
                         2, 2, 8, cid),
    mTextures(GameTextures::buildings())  {
    GameTextures::loadCardingShed();
}

std::vector<Overlay> eCardingShed::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::interface()[sizeId];
    auto os = eShepherBuildingBase::getOverlays(size);
    if(resource() > 0) {
        const int res = std::clamp((resource() + 1)/2, 0, 4);

        for(int i = 0; i < res; i++) {
            Overlay fleece;
            fleece.fTex = texs.fFleeceUnit;
            fleece.fX = 0.3 - i*0.2 + (i > 1 ? 0.5 : 0);
            fleece.fY = -1.6 - i*0.2;
            os.push_back(fleece);
        }
    }
    return os;
}

