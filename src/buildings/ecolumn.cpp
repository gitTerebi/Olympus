#include "ecolumn.h"

#include "textures/game-textures.h"
#include "textures/building-textures.h"
#include "engine/game-board.h"

eDoricColumn::eDoricColumn(GameBoard& board, const eCityId cid) :
    eColumn(board, &BuildingTextures::fDoricColumn,
            eBuildingType::doricColumn, 1, 1, cid) {
    GameTextures::loadColumns();
}

eIonicColumn::eIonicColumn(GameBoard& board, const eCityId cid) :
    eColumn(board, &BuildingTextures::fIonicColumn,
            eBuildingType::ionicColumn, 1, 1, cid) {
    GameTextures::loadColumns();
}

eCorinthianColumn::eCorinthianColumn(GameBoard& board, const eCityId cid) :
    eColumn(board, &BuildingTextures::fCorinthianColumn,
            eBuildingType::corinthianColumn, 1, 1, cid) {
    GameTextures::loadColumns();
}

std::vector<Overlay> eColumn::getOverlays(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& bds = GameTextures::buildings();
    const auto& texs = bds[sizeId];
    const auto t = centerTile();
    auto& board = getBoard();
    const auto dir = board.direction();
    std::vector<Overlay> os;
    if(const auto bl = t->bottomLeftRotated<eTile>(dir)) {
        if(bl->underBuildingType() == type()) {
            os.push_back(Overlay{-1.95, -1.9, texs.fColumnConnectionH});
        }
    }
    if(const auto br = t->bottomRightRotated<eTile>(dir)) {
        if(br->underBuildingType() == type()) {
            os.push_back(Overlay{-1.45, -2.4, texs.fColumnConnectionW});
        }
    }
    return os;
}
