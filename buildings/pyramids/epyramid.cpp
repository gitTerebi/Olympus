#include "epyramid.h"

#include "epyramidwall.h"
#include "epyramidtop.h"
#include "epyramidtile.h"
#include "epyramidaltar.h"

#include "engine/egameboard.h"

ePyramid::ePyramid(eGameBoard& board,
                   const eBuildingType type,
                   const int sw, const int sh,
                   const eCityId cid) :
    eMonument(board, type, sw, sh, 0, cid) {
    eGameTextures::loadPyramid();
    eGameTextures::loadSanctuary();
}

void ePyramid::erase() {
    eMonument::erase();
    mSelf.reset();
}

struct ePyramidWallData {
    eOrientation fO;
    int fElevation;
    int fX;
    int fY;
    int fSpecial;
};

enum class ePyramidTopType {
    top, tile, statue, monument,
    altar, temple, observatory, museum
};

struct ePyramidTopData {
    ePyramidTopType fType;
    int fElevation;
    int fX;
    int fY;
    int fSpecial = 0;
};

void ePyramid::initialize() {
    mSelf = ref<ePyramid>();
    auto& board = getBoard();
    const auto cid = cityId();
    const auto& rect = tileRect();
    std::vector<ePyramidWallData> walls;
    std::vector<ePyramidTopData> top;
    switch(type()) {
    case eBuildingType::modestPyramid: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 1},
            {eOrientation::right, 0, 2, 0, 0},
            {eOrientation::bottomRight, 0, 2, 1, 1},
            {eOrientation::bottom, 0, 2, 2, 0},
            {eOrientation::bottomLeft, 0, 1, 2, 1},
            {eOrientation::left, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 1}
        };

        top = {
            {ePyramidTopType::top, 1, 1, 1}
        };
    } break;
    case eBuildingType::pyramid: {
        for(int i = 0; i < 3; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 1},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 1},
            {eOrientation::right, 0, 4, 0, 0},
            {eOrientation::bottomRight, 0, 4, 1, 1},
            {eOrientation::bottomRight, 0, 4, 2, 0},
            {eOrientation::bottomRight, 0, 4, 3, 1},
            {eOrientation::bottom, 0, 4, 4, 0},
            {eOrientation::bottomLeft, 0, 3, 4, 1},
            {eOrientation::bottomLeft, 0, 2, 4, 0},
            {eOrientation::bottomLeft, 0, 1, 4, 1},
            {eOrientation::left, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 1},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 1},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::right, 1, 3, 1, 0},
            {eOrientation::bottomRight, 1, 3, 2, 0},
            {eOrientation::bottom, 1, 3, 3, 0},
            {eOrientation::bottomLeft, 1, 2, 3, 0},
            {eOrientation::left, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 0}
        };

        top = {
            {ePyramidTopType::top, 2, 2, 2}
        };
    } break;
    case eBuildingType::greatPyramid: {
        for(int i = 0; i < 4; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 1},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 1},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::right, 0, 6, 0, 0},
            {eOrientation::bottomRight, 0, 6, 1, 0},
            {eOrientation::bottomRight, 0, 6, 2, 1},
            {eOrientation::bottomRight, 0, 6, 3, 0},
            {eOrientation::bottomRight, 0, 6, 4, 1},
            {eOrientation::bottomRight, 0, 6, 5, 0},
            {eOrientation::bottom, 0, 6, 6, 0},
            {eOrientation::bottomLeft, 0, 5, 6, 0},
            {eOrientation::bottomLeft, 0, 4, 6, 1},
            {eOrientation::bottomLeft, 0, 3, 6, 0},
            {eOrientation::bottomLeft, 0, 2, 6, 1},
            {eOrientation::bottomLeft, 0, 1, 6, 0},
            {eOrientation::left, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 1},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 1},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 0},
            {eOrientation::topRight, 1, 4, 1, 0},
            {eOrientation::right, 1, 5, 1, 0},
            {eOrientation::bottomRight, 1, 5, 2, 0},
            {eOrientation::bottomRight, 1, 5, 3, 0},
            {eOrientation::bottomRight, 1, 5, 4, 0},
            {eOrientation::bottom, 1, 5, 5, 0},
            {eOrientation::bottomLeft, 1, 4, 5, 0},
            {eOrientation::bottomLeft, 1, 3, 5, 0},
            {eOrientation::bottomLeft, 1, 2, 5, 0},
            {eOrientation::left, 1, 1, 5, 0},
            {eOrientation::topLeft, 1, 1, 4, 0},
            {eOrientation::topLeft, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 0},

            {eOrientation::top, 2, 2, 2, 0},
            {eOrientation::topRight, 2, 3, 2, 1},
            {eOrientation::right, 2, 4, 2, 0},
            {eOrientation::bottomRight, 2, 4, 3, 1},
            {eOrientation::bottom, 2, 4, 4, 0},
            {eOrientation::bottomLeft, 2, 3, 4, 1},
            {eOrientation::left, 2, 2, 4, 0},
            {eOrientation::topLeft, 2, 2, 3, 1}
        };

        top = {
            {ePyramidTopType::top, 3, 3, 3}
        };
    } break;
    case eBuildingType::majesticPyramid: {
        for(int i = 0; i < 5; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 1},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::topRight, 0, 6, 0, 0},
            {eOrientation::topRight, 0, 7, 0, 0},
            {eOrientation::right, 0, 8, 0, 0},
            {eOrientation::bottomRight, 0, 8, 1, 0},
            {eOrientation::bottomRight, 0, 8, 2, 0},
            {eOrientation::bottomRight, 0, 8, 3, 0},
            {eOrientation::bottomRight, 0, 8, 4, 1},
            {eOrientation::bottomRight, 0, 8, 5, 0},
            {eOrientation::bottomRight, 0, 8, 6, 0},
            {eOrientation::bottomRight, 0, 8, 7, 0},
            {eOrientation::bottom, 0, 8, 8, 0},
            {eOrientation::bottomLeft, 0, 7, 8, 0},
            {eOrientation::bottomLeft, 0, 6, 8, 0},
            {eOrientation::bottomLeft, 0, 5, 8, 0},
            {eOrientation::bottomLeft, 0, 4, 8, 1},
            {eOrientation::bottomLeft, 0, 3, 8, 0},
            {eOrientation::bottomLeft, 0, 2, 8, 0},
            {eOrientation::bottomLeft, 0, 1, 8, 0},
            {eOrientation::left, 0, 0, 8, 0},
            {eOrientation::topLeft, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 1},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 0},
            {eOrientation::topRight, 1, 4, 1, 1},
            {eOrientation::topRight, 1, 5, 1, 0},
            {eOrientation::topRight, 1, 6, 1, 0},
            {eOrientation::right, 1, 7, 1, 0},
            {eOrientation::bottomRight, 1, 7, 2, 0},
            {eOrientation::bottomRight, 1, 7, 3, 0},
            {eOrientation::bottomRight, 1, 7, 4, 1},
            {eOrientation::bottomRight, 1, 7, 5, 0},
            {eOrientation::bottomRight, 1, 7, 6, 0},
            {eOrientation::bottom, 1, 7, 7, 0},
            {eOrientation::bottomLeft, 1, 6, 7, 0},
            {eOrientation::bottomLeft, 1, 5, 7, 0},
            {eOrientation::bottomLeft, 1, 4, 7, 1},
            {eOrientation::bottomLeft, 1, 3, 7, 0},
            {eOrientation::bottomLeft, 1, 2, 7, 0},
            {eOrientation::left, 1, 1, 7, 0},
            {eOrientation::topLeft, 1, 1, 6, 0},
            {eOrientation::topLeft, 1, 1, 5, 0},
            {eOrientation::topLeft, 1, 1, 4, 1},
            {eOrientation::topLeft, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 0},

            {eOrientation::top, 2, 2, 2, 0},
            {eOrientation::topRight, 2, 3, 2, 0},
            {eOrientation::topRight, 2, 4, 2, 1},
            {eOrientation::topRight, 2, 5, 2, 0},
            {eOrientation::right, 2, 6, 2, 0},
            {eOrientation::bottomRight, 2, 6, 3, 0},
            {eOrientation::bottomRight, 2, 6, 4, 1},
            {eOrientation::bottomRight, 2, 6, 5, 0},
            {eOrientation::bottom, 2, 6, 6, 0},
            {eOrientation::bottomLeft, 2, 5, 6, 0},
            {eOrientation::bottomLeft, 2, 4, 6, 1},
            {eOrientation::bottomLeft, 2, 3, 6, 0},
            {eOrientation::left, 2, 2, 6, 0},
            {eOrientation::topLeft, 2, 2, 5, 0},
            {eOrientation::topLeft, 2, 2, 4, 1},
            {eOrientation::topLeft, 2, 2, 3, 0},

            {eOrientation::top, 3, 3, 3, 0},
            {eOrientation::topRight, 3, 4, 3, 1},
            {eOrientation::right, 3, 5, 3, 0},
            {eOrientation::bottomRight, 3, 5, 4, 1},
            {eOrientation::bottom, 3, 5, 5, 0},
            {eOrientation::bottomLeft, 3, 4, 5, 1},
            {eOrientation::left, 3, 3, 5, 0},
            {eOrientation::topLeft, 3, 3, 4, 1}
        };

        top = {
            {ePyramidTopType::top, 4, 4, 4}
        };
    } break;
    case eBuildingType::smallMonumentToTheSky: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 3},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::right, 0, 4, 0, 0},
            {eOrientation::bottomRight, 0, 4, 1, 0},
            {eOrientation::bottomRight, 0, 4, 2, 2},
            {eOrientation::bottomRight, 0, 4, 3, 0},
            {eOrientation::bottom, 0, 4, 4, 0},
            {eOrientation::bottomLeft, 0, 3, 4, 0},
            {eOrientation::bottomLeft, 0, 2, 4, 3},
            {eOrientation::bottomLeft, 0, 1, 4, 0},
            {eOrientation::left, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 2},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 3},
            {eOrientation::right, 1, 3, 1, 0},
            {eOrientation::bottomRight, 1, 3, 2, 0},
            {eOrientation::bottom, 1, 3, 3, 0},
            {eOrientation::bottomLeft, 1, 2, 3, 3},
            {eOrientation::left, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 0}
        };

        top = {
            {ePyramidTopType::tile, 2, 2, 2}
        };
    } break;
    case eBuildingType::monumentToTheSky: {
        for(int i = 0; i < 3; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 2},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 2},
            {eOrientation::right, 0, 5, 0, 0},
            {eOrientation::bottomRight, 0, 5, 1, 0},
            {eOrientation::bottomRight, 0, 5, 2, 3},
            {eOrientation::bottomRight, 0, 5, 3, 3},
            {eOrientation::bottomRight, 0, 5, 4, 0},
            {eOrientation::bottom, 0, 5, 5, 0},
            {eOrientation::bottomLeft, 0, 4, 5, 2},
            {eOrientation::bottomLeft, 0, 3, 5, 0},
            {eOrientation::bottomLeft, 0, 2, 5, 0},
            {eOrientation::bottomLeft, 0, 1, 5, 2},
            {eOrientation::left, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 3},
            {eOrientation::topLeft, 0, 0, 2, 3},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 0},
            {eOrientation::right, 1, 4, 1, 0},
            {eOrientation::bottomRight, 1, 4, 2, 3},
            {eOrientation::bottomRight, 1, 4, 3, 3},
            {eOrientation::bottom, 1, 4, 4, 0},
            {eOrientation::bottomLeft, 1, 3, 4, 0},
            {eOrientation::bottomLeft, 1, 2, 4, 0},
            {eOrientation::left, 1, 1, 4, 0},
            {eOrientation::topLeft, 1, 1, 3, 3},
            {eOrientation::topLeft, 1, 1, 2, 3}
        };

        top = {
            {ePyramidTopType::tile, 2, 2, 2},
            {ePyramidTopType::tile, 2, 3, 2},
            {ePyramidTopType::tile, 2, 2, 3},
            {ePyramidTopType::tile, 2, 3, 3}
        };
    } break;
    default:
        break;
    }

    for(const auto& w : walls) {
        const auto b = e::make_shared<ePyramidWall>(
                           this, board, w.fO, w.fElevation,
                           w.fSpecial, cid);

        const int tx = rect.x + w.fX;
        const int ty = rect.y + w.fY;
        const auto tile = board.tile(tx, ty);
        tile->setUnderBuilding(b);
        b->setCenterTile(tile);
        b->addUnderBuilding(tile);
        b->setTileRect({tx, ty, 1, 1});

        b->setMonument(this);
        registerElement(b);
    }

    for(const auto& t : top) {
        stdsptr<ePyramidElement> b;
        switch(t.fType) {
        case ePyramidTopType::top: {
            b = e::make_shared<ePyramidTop>(
                    this, board, t.fElevation,
                    cid);
        } break;
        case ePyramidTopType::tile: {
            b = e::make_shared<ePyramidTile>(
                    this, board, t.fElevation,
                    t.fSpecial, cid);
        } break;
        case ePyramidTopType::altar: {
            b = e::make_shared<ePyramidAltar>(
                    this, board, t.fElevation,
                    cid);
        } break;
        }

        const int tx = rect.x + t.fX;
        const int ty = rect.y + t.fY;
        const auto tile = board.tile(tx, ty);
        tile->setUnderBuilding(b);
        b->setCenterTile(tile);
        b->addUnderBuilding(tile);
        b->setTileRect({tx, ty, 1, 1});

        b->setMonument(this);
        registerElement(b);
    }

    const auto c = cost(); // !!!
    add(eResourceType::marble, c.fMarble);
    add(eResourceType::wood, c.fWood);
    add(eResourceType::sculpture, c.fSculpture);
    add(eResourceType::orichalc, c.fOrichalc);
    add(eResourceType::blackMarble, c.fBlackMarble);
}

void ePyramid::buildingProgressed() {
    int minLevel = 10000;
    for(const auto& e : mElements) {
        const auto pe = static_cast<ePyramidElement*>(e.get());
        const bool f = pe->finished();
        if(f) continue;
        const int level = pe->currentElevation();
        if(minLevel > level) minLevel = level;
    }
    for(const auto& e : mElements) {
        const auto pe = static_cast<ePyramidElement*>(e.get());
        const bool f = pe->finished();
        if(f) {
            pe->setHalted(false);
            continue;
        }
        const int level = pe->currentElevation();
        pe->setHalted(level > minLevel);
    }
}

void ePyramid::read(eReadStream& src) {
    eMonument::read(src);
    mSelf = ref<ePyramid>();
}

eSanctCost ePyramid::swapMarbleIfDark(const int e, eSanctCost cost) const {
    const bool isDark = darkLevel(e);
    if(isDark) cost.switchMarble();
    return cost;
}

void ePyramid::sDimensions(const eBuildingType type, int& sw, int& sh) {
    switch(type) {
    case eBuildingType::modestPyramid:
        sw = 3;
        sh = 3;
        break;
    case eBuildingType::pyramid:
        sw = 5;
        sh = 5;
        break;
    case eBuildingType::greatPyramid:
        sw = 7;
        sh = 7;
        break;
    case eBuildingType::majesticPyramid:
        sw = 9;
        sh = 9;
        break;

    case eBuildingType::smallMonumentToTheSky:
        sw = 5;
        sh = 5;
        break;
    case eBuildingType::monumentToTheSky:
        sw = 6;
        sh = 6;
        break;
    case eBuildingType::grandMonumentToTheSky:
        sw = 8;
        sh = 8;
        break;

    case eBuildingType::minorShrine:
        sw = 3;
        sh = 3;
        break;
    case eBuildingType::shrine:
        sw = 6;
        sh = 6;
        break;
    case eBuildingType::majorShrine:
        sw = 8;
        sh = 8;
        break;

    case eBuildingType::pyramidToThePantheon:
        sw = 11;
        sh = 9;
        break;
    case eBuildingType::altarOfOlympus:
        sw = 8;
        sh = 8;
        break;
    case eBuildingType::templeOfOlympus:
        sw = 8;
        sh = 8;
        break;
    case eBuildingType::observatoryKosmika:
        sw = 9;
        sh = 9;
        break;
    case eBuildingType::museumAtlantika:
        sw = 8;
        sh = 8;
        break;
    default:
        break;
    }
}
