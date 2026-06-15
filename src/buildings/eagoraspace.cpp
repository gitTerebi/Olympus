#include "eagoraspace.h"

#include "textures/game-textures.h"

eAgoraSpace::eAgoraSpace(const stdsptr<eAgoraBase>& agora,
                         GameBoard& board, const eCityId cid) :
    eBuilding(board, eBuildingType::agoraSpace, 2, 2, cid),
    mAgora(agora) {}

stdsptr<eTexture> eAgoraSpace::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& texs = GameTextures::buildings();
    const auto& coll = texs[sizeId].fAgora;
    return coll.getTexture(seed() % coll.size());
}
