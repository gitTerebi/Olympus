#include "eolivepress.h"

#include "textures/egametextures.h"
#include "enumbers.h"
#include <algorithm>

eOlivePress::eOlivePress(eGameBoard& board,
                         const eCityId cid) :
    eProcessingBuilding(board,
                        &eBuildingTextures::fOlivePress,
                        -3.65, -3.75,
                        &eBuildingTextures::fOlivePressOverlay,
                        eBuildingType::olivePress, 2, 2, 12,
                        eResourceType::olives,
                        eResourceType::oliveOil, 1,
                        eNumbers::sOlivePressProcessingPeriod,
                        cid) {
    eGameTextures::loadOlivePress();
}

std::vector<eOverlay> eOlivePress::getOverlays(const eTileSize size) const {
    auto os = eProcessingBuilding::getOverlays(size);
    const int olives = rawCount();
    if(olives > 0) {
        const int sizeId = static_cast<int>(size);
        const auto& texs = eGameTextures::buildings()[sizeId];
        const auto& coll = texs.fWaitingOlives;
        const int resMax = coll.size() - 1;
        const int res = std::clamp(olives - 1, 0, resMax);
        eOverlay o;
        o.fTex = coll.getTexture(res);
        o.fX = -0.2;
        o.fY = -1.85;
        os.push_back(o);
    }
    return os;
}
