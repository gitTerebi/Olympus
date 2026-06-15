#include "eolivepress.h"

#include "textures/game-textures.h"
#include "enumbers.h"
#include <algorithm>

namespace {
constexpr int sIdleFacing = 3;
constexpr double sIdleX = 0.5;
constexpr double sIdleY = -1.5;
}

eOlivePress::eOlivePress(GameBoard& board,
                         const eCityId cid) :
    eProcessingBuilding(board,
                        &BuildingTextures::fOlivePress,
                        -3.65, -3.75,
                        &BuildingTextures::fOlivePressOverlay,
                        eBuildingType::olivePress, 2, 2, 12,
                        eResourceType::olives,
                        eResourceType::oliveOil, 1,
                        eNumbers::sOlivePressProcessingPeriod,
                        cid) {
    GameTextures::loadOlivePress();
    GameTextures::loadWaitingOverlay();
    setOverlayEnabledFunc([this]() {
        return enabled();
    });
}

std::vector<eOverlay> eOlivePress::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int olives = rawCount();
    if(olives > 0) {
        os = eProcessingBuilding::getOverlays(size);
        const int sizeId = static_cast<int>(size);
        const auto& texs = GameTextures::buildings()[sizeId];
        const auto& coll = texs.fWaitingOlives;
        const int resMax = coll.size() - 1;
        const int res = std::clamp(olives - 1, 0, resMax);
        eOverlay o;
        o.fTex = coll.getTexture(res);
        o.fX = -0.5;
        o.fY = -2.10;
        os.push_back(o);
    } else {
        const int sizeId = static_cast<int>(size);
        const auto& texs = GameTextures::buildings()[sizeId];
        const auto& colls = seed() % 2 ?
                            texs.fWaitingOverlay0 :
                            texs.fWaitingOverlay1;
        if(sIdleFacing < static_cast<int>(colls.size())) {
            const auto& coll = colls[sIdleFacing];
            if(coll.size() > 0) {
                eOverlay o;
                o.fTex = coll.getTexture(textureTime() % coll.size());
                o.fX = sIdleX;
                o.fY = sIdleY;
                os.push_back(o);
            }
        }
    }
    return os;
}
