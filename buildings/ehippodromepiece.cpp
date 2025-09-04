#include "ehippodromepiece.h"

#include "etilehelper.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "buildings/eroad.h"
#include "ehippodrome.h"

eHippodromePiece::eHippodromePiece(eGameBoard &board, const eCityId cid) :
    eBuilding(board, eBuildingType::hippodromePiece, 4, 4, cid) {
    eGameTextures::loadHippodrome();
    setEnabled(true);
}

eHippodromePiece::~eHippodromePiece() {
    if(mHippodrome) mHippodrome->clear();
}

using eN = eHippodromePiece::eNeighbour;
std::vector<eN> eHippodromePiece::neighbours() const {
    std::vector<eN> result;
    const auto& r = tileRect();
    const auto& board = getBoard();
    const auto tlb = board.buildingAt(r.x - 1, r.y);
    const auto trb = board.buildingAt(r.x, r.y - 1);
    const auto brb = board.buildingAt(r.x + r.w, r.y);
    const auto blb = board.buildingAt(r.x, r.y + r.h);

    if(const auto tlh = dynamic_cast<eHippodromePiece*>(tlb)) {
        const int id = tlh->id();
        if(id == 0 || id == 4 || id == 5 || id == 7) {
            result.push_back({tlh, eDiagonalOrientation::topLeft});
        }
    }
    if(const auto trh = dynamic_cast<eHippodromePiece*>(trb)) {
        const int id = trh->id();
        if(id == 1 || id == 2 || id == 6 || id == 7) {
            result.push_back({trh, eDiagonalOrientation::topRight});
        }
    }
    if(const auto brh = dynamic_cast<eHippodromePiece*>(brb)) {
        const int id = brh->id();
        if(id == 0 || id == 1 || id == 3 || id == 4) {
            result.push_back({brh, eDiagonalOrientation::bottomRight});
        }
    }
    if(const auto blh = dynamic_cast<eHippodromePiece*>(blb)) {
        const int id = blh->id();
        if(id == 2 || id == 3 || id == 5 || id == 6) {
            result.push_back({blh, eDiagonalOrientation::bottomLeft});
        }
    }

    return result;
}

void eHippodromePiece::progressPath(std::vector<ePathPoint>& path) const {
    if(path.empty()) return;
    const auto last = path.back();
    const auto& r = tileRect();
    if(mId == 0 || mId == 4) {
        if(last.fX <= r.x) {
            path.push_back({double(r.x + r.w - 1), last.fY, 0.});
        } else {
            path.push_back({double(r.x), last.fY, 0.});
        }
    } else if(mId == 1) {
        if(last.fX <= r.x) {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x), last.fY, 0.});
            path.push_back({r.x + r.w - 1 - dy, double(r.y + r.h - 1), 0.});
        } else {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y + r.h - 1), 0.});
            path.push_back({double(r.x), r.y + r.h - 1 - dx, 0.});
        }
    } else if(mId == 2 || mId == 6) {
        if(last.fY <= r.y) {
            path.push_back({last.fX, double(r.y + r.h - 1), 0.});
        } else {
            path.push_back({last.fX, double(r.y), 0.});
        }
    } else if(mId == 3) {
        if(last.fX <= r.x) {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x), last.fY, 0.});
            path.push_back({r.x + dy, double(r.y), 0.});
        } else {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y), 0.});
            path.push_back({double(r.x), r.y + dx, 0.});
        }
    } else if(mId == 5) {
        if(last.fY <= r.y) {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y), 0.});
            path.push_back({double(r.x + r.w - 1), r.y + r.h - 1 - dx, 0.});
        } else {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x + r.w - 1), last.fY, 0.});
            path.push_back({r.x + r.w - 1 - dy, double(r.y), 0.});
        }
    } else if(mId == 7) {
        if(last.fY >= r.y + r.h - 1) {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y + r.h - 1), 0.});
            path.push_back({double(r.x + r.w - 1), r.y + dx, 0.});
        } else {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x + r.w - 1), last.fY, 0.});
            path.push_back({r.x + dy, double(r.y + r.h - 1), 0.});
        }
    } else {
        assert(false);
    }
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

void eHippodromePiece::horseTile(eTile* const t,
                                 std::vector<eOverlay>& result,
                                 const eWorldDirection dir,
                                 const eTileSize size,
                                 const SDL_Rect& rr) const {
    const auto& ms = t->missiles();
    for(const auto& m : ms) {
        const auto type = m->type();
        if(type != eMissileType::racingHorse) continue;
        const auto tex = m->getTexture(size);
        auto& o = result.emplace_back();
        o.fTex = tex;
        const double mx = m->globalX();
        const double my = m->globalY();
        switch(dir) {
        case eWorldDirection::N: {
            o.fX = mx - rr.x - 1;
            o.fY = my - rr.y - 5;
        } break;
        case eWorldDirection::E: {
            const double x = mx - rr.x + 2;
            const double y = my - rr.y - 1;

            o.fX = y;
            o.fY = -x;
        } break;
        case eWorldDirection::S: {
            const double x = mx - rr.x - 2;
            const double y = my - rr.y + 2;

            o.fX = -x;
            o.fY = -y;
        } break;
        case eWorldDirection::W: {
            const double x = mx - rr.x - 5;
            const double y = my - rr.y - 2;

            o.fX = -y;
            o.fY = x;
        } break;
        }
    }
}

std::vector<eOverlay> eHippodromePiece::getOverlays(const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const auto& rr = tileRect();
    std::vector<eOverlay> result;
    switch(dir) {
    case eWorldDirection::N: {
        for(int y = rr.y - 1; y < rr.y + rr.h - 1; y++) {
            for(int x = rr.x - 1; x < rr.x + rr.w - 1; x++) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                horseTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::E: {
        for(int x = rr.x + rr.w - 1 + 1; x >= rr.x + 1; x--) {
            for(int y = rr.y - 1; y < rr.y + rr.h - 1; y++) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                horseTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::S: {
        for(int y = rr.y + rr.h - 1 + 1; y >= rr.y + 1; y--) {
            for(int x = rr.x + rr.w - 1 + 1; x >= rr.x + 1; x--) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                horseTile(t, result, dir, size, rr);
            }
        }
    } break;
    case eWorldDirection::W: {
        for(int x = rr.x - 1; x < rr.x + rr.w - 1; x++) {
            for(int y = rr.y + rr.h - 1 + 1; y >= rr.y + 1; y--) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                horseTile(t, result, dir, size, rr);
            }
        }
    } break;
    }
    if(mId == 2 || mId == 4) {
        switch(dir) {
        case eWorldDirection::N: {
            for(int y = rr.y; y < rr.y + rr.h; y++) {
                for(int x = rr.x; x < rr.x + rr.w; x++) {
                    const auto t = board.tile(x, y);
                    if(!t) continue;
                    handleTile(t, result, dir, size, rr);
                }
            }
        } break;
        case eWorldDirection::E: {
            for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
                for(int y = rr.y; y < rr.y + rr.h; y++) {
                    const auto t = board.tile(x, y);
                    if(!t) continue;
                    handleTile(t, result, dir, size, rr);
                }
            }
        } break;
        case eWorldDirection::S: {
            for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
                for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
                    const auto t = board.tile(x, y);
                    if(!t) continue;
                    handleTile(t, result, dir, size, rr);
                }
            }
        } break;
        case eWorldDirection::W: {
            for(int x = rr.x; x < rr.x + rr.w; x++) {
                for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
                    const auto t = board.tile(x, y);
                    if(!t) continue;
                    handleTile(t, result, dir, size, rr);
                }
            }
        } break;
        }
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
