#include "epier.h"

#include "textures/egametextures.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

ePier::ePier(eGameBoard& board, const eDiagonalOrientation o,
             const eCityId cid) :
    eBuilding(board, eBuildingType::pier, 2, 2, cid),
    mO(o) {
    eGameTextures::loadPier();
}

std::shared_ptr<eTexture> ePier::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& coll = blds[sizeId].fPier1;
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    int id = 3;
    switch(o) {
    case eDiagonalOrientation::topRight:
        id = 0;
        break;
    case eDiagonalOrientation::bottomRight:
        id = 1;
        break;
    case eDiagonalOrientation::bottomLeft:
        id = 2;
        break;
    case eDiagonalOrientation::topLeft:
        id = 3;
        break;
    default:
        id = 0;
        break;
    }

    return coll.getTexture(id);
}

void ePier::collapse() {
    if(mTradePost) {
        mTradePost->collapse();
    } else {
        eBuilding::collapse();
    }
}

void ePier::setTradePost(eBuilding* const b) {
    mTradePost = b;
}

void ePier::serializeFields(eSaveArchive& ar) {
    eBuilding::serializeFields(ar);
    ar.buildingAsField("tradePost", &getBoard(), mTradePost);
}
