#include "ehippodromepiece.h"

#include "etilehelper.h"
#include "textures/egametextures.h"
#include "engine/egameboard.h"
#include "buildings/eroad.h"
#include "ehippodrome.h"

eHippodromePiece::eHippodromePiece(eGameBoard &board, const eCityId cid) :
    eBuildingWithResource(board, eBuildingType::hippodromePiece, 4, 4, cid) {
    eGameTextures::loadHippodrome();
    eGameTextures::loadHippodromeSpectators();
    eGameTextures::loadHippodromeFeces();
    eGameTextures::loadHippodromeFinish();
    setEnabled(true);
}

eHippodromePiece::~eHippodromePiece() {
    if(mHippodrome) mHippodrome->clear();
    if(mCart) mCart->kill();
}

int eHippodromePiece::add(const eResourceType type, const int count) {
    if(!mHippodrome) return 0;
    if(!static_cast<bool>(type & eResourceType::horse)) return 0;
    mHippodrome->addHorses(count);
    return count;
}

int eHippodromePiece::take(const eResourceType type, const int count) {
    (void)type;
    (void)count;
    return 0;
}

int eHippodromePiece::count(const eResourceType type) const {
    (void)type;
    return 0;
}

int eHippodromePiece::spaceLeft(const eResourceType type) const {
    if(!mHippodrome) return 0;
    if(type != eResourceType::horse) return 0;
    const int needed = mHippodrome->neededHorses();
    const int has = mHippodrome->hasHorses();
    return needed - has;
}

std::vector<eCartTask> eHippodromePiece::cartTasks() const {
    const int space = spaceLeft(eResourceType::horse);
    if(space <= 0) return {};
    return {{eCartActionType::take,
             eResourceType::horse,
             space}};
}

void eHippodromePiece::timeChanged(const int by) {
    (void)by;
    if(!mHippodrome) return;

    if(mHippodrome->working() && !mHippodrome->racing() &&
        mHippodrome->cleaningPartId() == mPartId) {
        if(mCleaningTimeShift == 0) {
            mCleaningTimeShift = -textureTime();
        } else {
            const int time = textureTime();
            const int cleaningTime = mCleaningTimeShift + time;
            if(cleaningTime > 200) {
                mCleaningTimeShift = 0;
                mHippodrome->nextCleaningPartId();
            }
        }
    }

    const bool c = mHippodrome->closed();
    const bool r = accessToRoad();
    if(mCart && (!c || !r)) {
        mCart->kill();
        return;
    }
    if(!c) return;
    const bool h = mHippodrome->hasCart();
    if(h) return;
    if(!r) return;
    if(!mCart) {
        mCart = spawnCart(eCartActionTypeSupport::take);
        mCart->setType(eCartTransporterType::horse);
        mHippodrome->setCart(mCart);
    }
}

using eN = eHippodromePiece::eNeighbour;
std::vector<eN> eHippodromePiece::neighbours() const {
    std::vector<eN> result;
    const auto& r = tileRect();
    const auto& board = getBoard();

    const auto hippodromeAt = [&](const int x, const int y) {
        const auto b = board.buildingAt(x, y);
        if(const auto r = dynamic_cast<eRoad*>(b)) {
            return r->aboveHippodrome();
        }
        return dynamic_cast<eHippodromePiece*>(b);
    };

    const auto tlh = hippodromeAt(r.x - 1, r.y);
    const auto trh = hippodromeAt(r.x, r.y - 1);
    const auto brh = hippodromeAt(r.x + r.w, r.y);
    const auto blh = hippodromeAt(r.x, r.y + r.h);

    if(tlh) {
        const int id = tlh->id();
        if(id == 0 || id == 4 || id == 5 || id == 7) {
            result.push_back({tlh, eDiagonalOrientation::topLeft});
        }
    }
    if(trh) {
        const int id = trh->id();
        if(id == 1 || id == 2 || id == 6 || id == 7) {
            result.push_back({trh, eDiagonalOrientation::topRight});
        }
    }
    if(brh) {
        const int id = brh->id();
        if(id == 0 || id == 1 || id == 3 || id == 4) {
            result.push_back({brh, eDiagonalOrientation::bottomRight});
        }
    }
    if(blh) {
        const int id = blh->id();
        if(id == 2 || id == 3 || id == 5 || id == 6) {
            result.push_back({blh, eDiagonalOrientation::bottomLeft});
        }
    }

    return result;
}

void eHippodromePiece::setHippodrome(eHippodrome * const h) {
    mHippodrome = h;
    if(mCart) mCart->kill();
}

struct eBezier1D {
    double fP0;
    double fC1;
    double fC2;
    double fP3;

    double at(const double t) {
        return fP0*pow(1 - t, 3.) + 3*fC1*t*pow(1 - t, 2.) + 3*fC2*t*t*(1 - t) + fP3*t*t*t;
    }
};

void bezierTo(std::vector<ePathPoint>& path,
              const double x, const double y,
              const bool clockwise) {
    if(path.empty()) return;
    const auto last = path.back();
    double cx;
    double cy;
    if(clockwise) {
        cx = x;
        cy = last.fY;
    } else {
        cx = last.fX;
        cy = y;
    }
    const double cx1 = last.fX + 0.6*(cx - last.fX);
    const double cy1 = last.fY + 0.6*(cy - last.fY);
    const double cx2 = x + 0.6*(cx - x);
    const double cy2 = y + 0.6*(cy - y);

    eBezier1D xb{last.fX, cx1, cx2, x};
    eBezier1D yb{last.fY, cy1, cy2, y};

    const int div = 10;
    const double inc = 1./div;
    for(int i = 1; i < div; i++) {
        const double t = i*inc;
        path.push_back({xb.at(t), yb.at(t), 0.});
    }
    path.push_back({x, y, 0.});
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
            bezierTo(path, r.x + r.w - 1 - dy, double(r.y + r.h - 1), true);
        } else {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y + r.h - 1), 0.});
            bezierTo(path, double(r.x), r.y + r.h - 1 - dx, false);
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
            bezierTo(path, r.x + dy, double(r.y), true);
        } else {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y), 0.});
            bezierTo(path, double(r.x), r.y + dx, false);
        }
    } else if(mId == 5) {
        if(last.fY <= r.y) {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y), 0.});
            bezierTo(path, double(r.x + r.w - 1), r.y + r.h - 1 - dx, false);
        } else {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x + r.w - 1), last.fY, 0.});
            bezierTo(path, r.x + r.w - 1 - dy, double(r.y), true);
        }
    } else if(mId == 7) {
        if(last.fY >= r.y + r.h - 1) {
            const double dx = last.fX - r.x;
            path.push_back({last.fX, double(r.y + r.h - 1), 0.});
            bezierTo(path, double(r.x + r.w - 1), r.y + dx, false);
        } else {
            const double dy = last.fY - r.y;
            path.push_back({double(r.x + r.w - 1), last.fY, 0.});
            bezierTo(path, r.x + dy, double(r.y + r.h - 1), true);
        }
    } else {
        assert(false);
    }
}

eTextureSpace eHippodromePiece::getTextureSpace(
    const int tx, const int ty,
    const eTileSize size) const {
    const auto& r = tileRect();
    const auto& board = getBoard();
    const auto dir = board.direction();
    int x;
    int y;
    switch(dir) {
    case eWorldDirection::N: {
        x = r.x;
        y = r.y;
    } break;
    case eWorldDirection::E: {
        x = r.x + r.w - 1;
        y = r.y;
    } break;
    case eWorldDirection::S: {
        x = r.x + r.w - 1;
        y = r.y + r.h - 1;
    } break;
    case eWorldDirection::W: {
        x = r.x;
        y = r.y + r.h - 1;
    } break;
    }

    if(tx == x && ty == y) {
        auto result = eBuilding::getTextureSpace(tx, ty, size);
        result.fX += 1.5;
        result.fY += 4.5;
        result.fRect = {x, y, 1, 1};
        result.fOvelays = false;
        return result;
    } else {
        const auto& ns = neighbours();
        bool rotatedBL = false;
        bool rotatedBR = false;
        for(const auto& n : ns) {
            const auto o = n.fO;
            const auto rotatedO = sRotated(o, dir);
            if(rotatedO == eDiagonalOrientation::bottomLeft) {
                rotatedBL = true;
            } else if(rotatedO == eDiagonalOrientation::bottomRight) {
                rotatedBR = true;
            }
        }

        int x;
        int y;
        switch(dir) {
        case eWorldDirection::N: {
            x = r.x + r.w - 1;
            y = r.y + r.h - 1;
        } break;
        case eWorldDirection::E: {
            x = r.x;
            y = r.y + r.h - 1;
        } break;
        case eWorldDirection::S: {
            x = r.x;
            y = r.y;
        } break;
        case eWorldDirection::W: {
            x = r.x + r.w - 1;
            y = r.y;
        } break;
        }
        if(!rotatedBL && !rotatedBR) {
            return eTextureSpace{nullptr, true, r};
        } else if(!rotatedBL || !rotatedBR) {
            if(rotatedBL) {
                if(tx == x) {
                    const SDL_Rect rr{x, r.y, 1, r.h};
                    return eTextureSpace{nullptr, true, rr, -2., 2.};
                } else {
                    return {nullptr};
                }
            }
            if(rotatedBR) {
                if(ty == y) {
                    const SDL_Rect rr{r.x, y, r.w, 1};
                    return eTextureSpace{nullptr, true, rr, -2., 2.};
                } else {
                    return {nullptr};
                }
            }
        }
        if(tx != x || ty != y) return {nullptr};
        const SDL_Rect rr{x, y, 1, 1};
        return eTextureSpace{nullptr, true, rr, -1.5, 1.5, false};
    }
    return {nullptr};
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

void eHippodromePiece::crossTile(eTile* const t,
                                 std::vector<eOverlay>& result,
                                 const eWorldDirection dir,
                                 const eTileSize size,
                                 const SDL_Rect& rr,
                                 const bool back) const {
    const bool hr = t->hasRoad();
    if(!hr) return;
    const auto ub = t->underBuilding();
    const auto r = static_cast<eRoad*>(ub);
    const auto h = r->aboveHippodrome();
    if(h != this) return;
    const int id = r->getHippodromeTextureId();
    if((id == 10 || id == 11) && !back) return;
    auto& o = result.emplace_back();
    const int sizeId = static_cast<int>(size);
    const auto& builTexs = eGameTextures::buildings()[sizeId];
    const auto& coll = builTexs.fHippodrome;
    o.fTex = coll.getTexture(id);
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

void iterateRenderOrder(const SDL_Rect& rr, const eGameBoard& board,
                        const eWorldDirection dir,
                        const std::function<void(eTile* const)>& func) {
    switch(dir) {
    case eWorldDirection::N: {
        for(int y = rr.y; y < rr.y + rr.h; y++) {
            for(int x = rr.x; x < rr.x + rr.w; x++) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                func(t);
            }
        }
    } break;
    case eWorldDirection::E: {
        for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
            for(int y = rr.y; y < rr.y + rr.h; y++) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                func(t);
            }
        }
    } break;
    case eWorldDirection::S: {
        for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
            for(int x = rr.x + rr.w - 1; x >= rr.x; x--) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                func(t);
            }
        }
    } break;
    case eWorldDirection::W: {
        for(int x = rr.x; x < rr.x + rr.w; x++) {
            for(int y = rr.y + rr.h - 1; y >= rr.y; y--) {
                const auto t = board.tile(x, y);
                if(!t) continue;
                func(t);
            }
        }
    } break;
    }
}

std::vector<eOverlay> eHippodromePiece::getOverlays(const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const auto& rr = tileRect();
    std::vector<eOverlay> result;

    const int sizeId = static_cast<int>(size);
    const auto& builTexs = eGameTextures::buildings()[sizeId];
    const auto& colls = builTexs.fHippodromeSpectators;
    const bool switchDir = mId == 6;
    const int time = textureTime();
    bool ns = dir == eWorldDirection::N ||
              dir == eWorldDirection::S;
    if(switchDir) ns = !ns;

    if(mHippodrome && mPartId == mHippodrome->finishPartId() && mHippodrome->working()) {
        const auto& colls = mHippodrome->racing() ? builTexs.fHippodromeFinishRacing :
                                                    builTexs.fHippodromeFinishNotRacing;
        auto& o = result.emplace_back();
        switch(dir) {
        case eWorldDirection::N: {
            const auto& coll = colls[0];
            const int texId = time % coll.size();
            o.fTex = coll.getTexture(texId);
            o.fX = -1.0;
            o.fY = -5.5;
        } break;
        case eWorldDirection::E: {
            const auto& coll = colls[3];
            const int texId = time % coll.size();
            o.fTex = coll.getTexture(texId);
            o.fX = -1.;
            o.fY = -2.;
        } break;
        case eWorldDirection::S: {
            const auto& coll = colls[2];
            const int texId = time % coll.size();
            o.fTex = coll.getTexture(texId);
            o.fX = 2.0;
            o.fY = -1.5;
        } break;
        case eWorldDirection::W: {
            const auto& coll = colls[1];
            const int texId = time % coll.size();
            o.fTex = coll.getTexture(texId);
            o.fX = 2.5;
            o.fY = -5.0;
        } break;
        }
    }

    if(mId == 2 || mId == 4) {
        iterateRenderOrder(rr, board, dir, [&](eTile* const t) {
            crossTile(t, result, dir, size, rr, true);
        });
    } else if(mId == 0 || mId == 6) {
        if(mHippodrome && mHippodrome->racing()) {
            if(ns) {
                const auto& coll = colls[1];
                const int id = time % coll.size();
                const auto& tex = coll.getTexture(id);
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = 0.0;
                    o.fY = -5.5;
                }
            } else  {
                const auto& coll = colls[0];
                const int id = time % coll.size();
                const auto& tex = coll.getTexture(id);
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = -1.25;
                    o.fY = -4.00;
                }
            }
        }
    }
    /*if(mId == 2 || mId == 4 || mId == 0 || mId == 6) */{
        if(mHippodrome && mHippodrome->working() && !mHippodrome->racing() &&
            mHippodrome->cleaningPartId() == mPartId) {
            const auto& colls = builTexs.fHippodromeFeces;
            const bool zero = mId == 0 || mId == 4;
            const auto& coll = zero ? colls[0] : colls[1];
            auto& o = result.emplace_back();
            const int cleaningTime = time + mCleaningTimeShift;
            if(cleaningTime < coll.size()) {
                o.fTex = coll.getTexture(cleaningTime);
            } else {
                const auto& colls = builTexs.fHippodromeFecesStanding;
                const bool zero = mId == 0 || mId == 4;
                const auto& coll = zero ? colls[0] : colls[1];
                o.fTex = coll.getTexture(cleaningTime % coll.size());
            }
            o.fX = -0.5;
            o.fY = -4.5;
        }
    }
    iterateRenderOrder(rr, board, dir, [&](eTile* const t) {
        horseTile(t, result, dir, size, rr);
    });
    if(mId == 2 || mId == 4) {
        iterateRenderOrder(rr, board, dir, [&](eTile* const t) {
            crossTile(t, result, dir, size, rr, false);
        });
    } else if(mId == 0 || mId == 6) {
        if(mHippodrome && mHippodrome->racing()) {
            if(ns) {
                const auto& coll = colls[2];
                const int id = time % coll.size();
                const auto& tex = coll.getTexture(id);
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = -0.5;
                    o.fY = -2.5;
                }
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = 1.5;
                    o.fY = -2.5;
                }
            } else  {
                const auto& coll = colls[3];
                const int id = time % coll.size();
                const auto& tex = coll.getTexture(id);
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = 1.75;
                    o.fY = -5.0;
                }
                {
                    auto& o = result.emplace_back();
                    o.fTex = tex;
                    o.fX = 1.75;
                    o.fY = -2.5;
                }
            }
        }
    }

    return result;
}

void eHippodromePiece::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    dst << mId;
    dst.writeCharacter(mCart);
}

void eHippodromePiece::read(eReadStream& src) {
    eBuilding::read(src);
    src >> mId;
    auto& board = getBoard();
    src.readCharacter(&board, [this](eCharacter* const c) {
        mCart = static_cast<eCartTransporter*>(c);
    });
}
