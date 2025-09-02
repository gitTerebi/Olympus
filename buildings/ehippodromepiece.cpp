#include "ehippodromepiece.h"

#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "buildings/eroad.h"

eHippodromePiece::eHippodromePiece(eGameBoard &board, const eCityId cid) :
    eBuilding(board, eBuildingType::hippodromePiece, 4, 4, cid) {
    eGameTextures::loadHippodrome();
    setEnabled(true);
}

std::shared_ptr<eTexture>
eHippodromePiece::getTexture(const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const std::map<eWorldDirection, std::map<int, int>> idMap = {
        {eWorldDirection::N, {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}}},
        {eWorldDirection::E, {{0, 6}, {1, 7}, {2, 4}, {3, 1}, {4, 2}, {5, 3}, {6, 0}, {7, 5}}},
        {eWorldDirection::S, {{0, 0}, {1, 5}, {2, 2}, {3, 7}, {4, 4}, {5, 1}, {6, 6}, {7, 3}}},
        {eWorldDirection::W, {{0, 6}, {1, 3}, {2, 4}, {3, 5}, {4, 2}, {5, 7}, {6, 0}, {7, 1}}}
    };
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& coll = blds[sizeId].fHippodrome;
    const int id = idMap.at(dir).at(mId);
    return coll.getTexture(id);
}

std::vector<eOverlay> eHippodromePiece::getOverlays(const eTileSize size) const {
    const auto& rr = tileRect();
    std::vector<eOverlay> result;
    const auto& tiles = tilesUnder();
    for(const auto t : tiles) {
        const bool hr = t->hasRoad();
        if(!hr) continue;
        const auto r = static_cast<eRoad*>(t->underBuilding());
        auto& o = result.emplace_back();
        o.fTex = r->getHippodromeTexture(size);
        o.fAlignTop = true;
        o.fX = t->x() - rr.x - 1.5;
        o.fY = t->y() - rr.y - 4.5;
    }
    return result;
}

void eHippodromePiece::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    dst << mId;
}

void eHippodromePiece::read(eReadStream& src) {
    eBuilding::read(src);
    src >> mId;
}
