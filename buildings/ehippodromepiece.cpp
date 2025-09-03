#include "ehippodromepiece.h"

#include "etilehelper.h"
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

void eHippodromePiece::handleTile(eTile* const t,
                                  std::vector<eOverlay>& result,
                                  const eWorldDirection dir,
                                  const eTileSize size,
                                  const SDL_Rect& rr) const {
    const bool hr = t->hasRoad();
    if(!hr) return;
    const auto ub = t->underBuilding();
    const auto r = static_cast<eRoad*>(ub);
    const auto h = r->aboveHippodrome();
    if(h != this) return;
    auto& o = result.emplace_back();
    o.fTex = r->getHippodromeTexture(size);
    o.fAlignTop = true;
    const int tx = t->x();
    const int ty = t->y();
    switch(dir) {
    case eWorldDirection::N: {
        o.fX = tx - rr.x - 1.5;
        o.fY = ty - rr.y - 4.5;
    } break;
    case eWorldDirection::E: {
        o.fX = ty - rr.y - 1.5;
        o.fY = rr.x - tx + rr.w - 1 - 4.5;
    } break;
    case eWorldDirection::S: {
        o.fX = rr.x - tx + rr.w - 1 - 1.5;
        o.fY = rr.y - ty + rr.h - 1 - 4.5;
    } break;
    case eWorldDirection::W: {
        o.fX = rr.y - ty + rr.h - 1 - 1.5;
        o.fY = tx - rr.x - 4.5;
    } break;
    }
}

std::vector<eOverlay> eHippodromePiece::getOverlays(const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const auto& rr = tileRect();
    std::vector<eOverlay> result;
    switch(dir) {
    case eWorldDirection::N: {
        for(int y = rr.y; y < rr.y + rr.h; y++) {
            for(int x = rr.x; x < rr.x + rr.w; x++) {
                const auto t = board.tile(x, y);
                handleTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::E: {
        for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
            for(int y = rr.y; y < rr.y + rr.h; y++) {
                const auto t = board.tile(x, y);
                handleTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::S: {
        for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
            for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
                const auto t = board.tile(x, y);
                handleTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::W: {
        for(int x = rr.x; x < rr.x + rr.w; x++) {
            for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
                const auto t = board.tile(x, y);
                handleTile(t, result, dir, size, rr);
            }
        }
    } break;
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
