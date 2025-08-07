#include "epyramid.h"

#include "epyramidwall.h"
#include "epyramidtop.h"
#include "epyramidtile.h"
#include "epyramidbuildingpart.h"
#include "epyramidaltar.h"
#include "epyramidstatue.h"
#include "epyramidmonument.h"
#include "epyramidobservatory.h"
#include "epyramidtemple.h"
#include "epyramidmuseum.h"

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
    int fSpecial2 = 0;
};

void ePyramid::initialize(const eGodType god) {
    mGod = god;
    const int godI = static_cast<int>(mGod);
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
        for(int i = 0; i < 2; i++) {
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
    case eBuildingType::grandMonumentToTheSky: {
        for(int i = 0; i < 3; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 2},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 3},
            {eOrientation::topRight, 0, 4, 0, 3},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::topRight, 0, 6, 0, 2},
            {eOrientation::right, 0, 7, 0, 0},
            {eOrientation::bottomRight, 0, 7, 1, 2},
            {eOrientation::bottomRight, 0, 7, 2, 0},
            {eOrientation::bottomRight, 0, 7, 3, 3},
            {eOrientation::bottomRight, 0, 7, 4, 3},
            {eOrientation::bottomRight, 0, 7, 5, 0},
            {eOrientation::bottomRight, 0, 7, 6, 2},
            {eOrientation::bottom, 0, 7, 7, 0},
            {eOrientation::bottomLeft, 0, 6, 7, 2},
            {eOrientation::bottomLeft, 0, 5, 7, 0},
            {eOrientation::bottomLeft, 0, 4, 7, 3},
            {eOrientation::bottomLeft, 0, 3, 7, 3},
            {eOrientation::bottomLeft, 0, 2, 7, 0},
            {eOrientation::bottomLeft, 0, 1, 7, 2},
            {eOrientation::left, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 2},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 3},
            {eOrientation::topLeft, 0, 0, 3, 3},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 2},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 3},
            {eOrientation::topRight, 1, 4, 1, 3},
            {eOrientation::topRight, 1, 5, 1, 0},
            {eOrientation::right, 1, 6, 1, 0},
            {eOrientation::bottomRight, 1, 6, 2, 0},
            {eOrientation::bottomRight, 1, 6, 3, 3},
            {eOrientation::bottomRight, 1, 6, 4, 3},
            {eOrientation::bottomRight, 1, 6, 5, 0},
            {eOrientation::bottom, 1, 6, 6, 0},
            {eOrientation::bottomLeft, 1, 5, 6, 0},
            {eOrientation::bottomLeft, 1, 4, 6, 3},
            {eOrientation::bottomLeft, 1, 3, 6, 3},
            {eOrientation::bottomLeft, 1, 2, 6, 0},
            {eOrientation::left, 1, 1, 6, 0},
            {eOrientation::topLeft, 1, 1, 5, 0},
            {eOrientation::topLeft, 1, 1, 4, 3},
            {eOrientation::topLeft, 1, 1, 3, 3},
            {eOrientation::topLeft, 1, 1, 2, 0},

            {eOrientation::top, 2, 2, 2, 0},
            {eOrientation::topRight, 2, 3, 2, 3},
            {eOrientation::topRight, 2, 4, 2, 3},
            {eOrientation::right, 2, 5, 2, 0},
            {eOrientation::bottomRight, 2, 5, 3, 3},
            {eOrientation::bottomRight, 2, 5, 4, 3},
            {eOrientation::bottom, 2, 5, 5, 0},
            {eOrientation::bottomLeft, 2, 4, 5, 3},
            {eOrientation::bottomLeft, 2, 3, 5, 3},
            {eOrientation::left, 2, 2, 5, 0},
            {eOrientation::topLeft, 2, 2, 4, 3},
            {eOrientation::topLeft, 2, 2, 3, 3}
        };

        top = {
            {ePyramidTopType::tile, 3, 3, 3},
            {ePyramidTopType::tile, 3, 4, 3},
            {ePyramidTopType::tile, 3, 3, 4},
            {ePyramidTopType::tile, 3, 4, 4}
        };
    } break;
    case eBuildingType::minorShrine: {
        for(int i = 0; i < 1; i++) {
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
            {ePyramidTopType::statue, 1, 1, 1, godI, 1}
        };
    } break;
    case eBuildingType::shrine: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 1},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 1},
            {eOrientation::right, 0, 5, 0, 0},
            {eOrientation::bottomRight, 0, 5, 1, 1},
            {eOrientation::bottomRight, 0, 5, 2, 0},
            {eOrientation::bottomRight, 0, 5, 3, 0},
            {eOrientation::bottomRight, 0, 5, 4, 1},
            {eOrientation::bottom, 0, 5, 5, 0},
            {eOrientation::bottomLeft, 0, 4, 5, 1},
            {eOrientation::bottomLeft, 0, 3, 5, 0},
            {eOrientation::bottomLeft, 0, 2, 5, 0},
            {eOrientation::bottomLeft, 0, 1, 5, 1},
            {eOrientation::left, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 1},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 1},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 0},
            {eOrientation::right, 1, 4, 1, 0},
            {eOrientation::bottomRight, 1, 4, 2, 0},
            {eOrientation::bottomRight, 1, 4, 3, 0},
            {eOrientation::bottom, 1, 4, 4, 0},
            {eOrientation::bottomLeft, 1, 3, 4, 0},
            {eOrientation::bottomLeft, 1, 2, 4, 0},
            {eOrientation::left, 1, 1, 4, 0},
            {eOrientation::topLeft, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 0}
        };

        top = {
            {ePyramidTopType::monument, 2, 3, 3, godI, 1}
        };
    } break;
    case eBuildingType::majorShrine: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 0},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::topRight, 0, 6, 0, 0},
            {eOrientation::right, 0, 7, 0, 0},
            {eOrientation::bottomRight, 0, 7, 1, 0},
            {eOrientation::bottomRight, 0, 7, 2, 0},
            {eOrientation::bottomRight, 0, 7, 3, 0},
            {eOrientation::bottomRight, 0, 7, 4, 0},
            {eOrientation::bottomRight, 0, 7, 5, 0},
            {eOrientation::bottomRight, 0, 7, 6, 0},
            {eOrientation::bottom, 0, 7, 7, 0},
            {eOrientation::bottomLeft, 0, 6, 7, 0},
            {eOrientation::bottomLeft, 0, 5, 7, 0},
            {eOrientation::bottomLeft, 0, 4, 7, 0},
            {eOrientation::bottomLeft, 0, 3, 7, 0},
            {eOrientation::bottomLeft, 0, 2, 7, 0},
            {eOrientation::bottomLeft, 0, 1, 7, 0},
            {eOrientation::left, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 2, 2, 0},
            {eOrientation::topRight, 1, 3, 2, 0},
            {eOrientation::topRight, 1, 4, 2, 0},
            {eOrientation::right, 1, 5, 2, 0},
            {eOrientation::bottomRight, 1, 5, 3, 0},
            {eOrientation::bottomRight, 1, 5, 4, 0},
            {eOrientation::bottom, 1, 5, 5, 0},
            {eOrientation::bottomLeft, 1, 4, 5, 0},
            {eOrientation::bottomLeft, 1, 3, 5, 0},
            {eOrientation::left, 1, 2, 5, 0},
            {eOrientation::topLeft, 1, 2, 4, 0},
            {eOrientation::topLeft, 1, 2, 3, 0}
        };

        top = {
            {ePyramidTopType::statue, 1, 1, 1, godI, 1},
            {ePyramidTopType::statue, 1, 6, 1, godI, 1},
            {ePyramidTopType::statue, 1, 1, 6, godI, 1},
            {ePyramidTopType::statue, 1, 6, 6, godI, 1},

            {ePyramidTopType::tile, 1, 1, 2, 0},
            {ePyramidTopType::tile, 1, 1, 3, 0},
            {ePyramidTopType::tile, 1, 1, 4, 0},
            {ePyramidTopType::tile, 1, 1, 5, 0},

            {ePyramidTopType::tile, 1, 2, 6, 0},
            {ePyramidTopType::tile, 1, 3, 6, 0},
            {ePyramidTopType::tile, 1, 4, 6, 0},
            {ePyramidTopType::tile, 1, 5, 6, 0},

            {ePyramidTopType::tile, 1, 2, 1, 0},
            {ePyramidTopType::tile, 1, 3, 1, 0},
            {ePyramidTopType::tile, 1, 4, 1, 0},
            {ePyramidTopType::tile, 1, 5, 1, 0},

            {ePyramidTopType::tile, 1, 6, 2, 0},
            {ePyramidTopType::tile, 1, 6, 3, 0},
            {ePyramidTopType::tile, 1, 6, 4, 0},
            {ePyramidTopType::tile, 1, 6, 5, 0},

            {ePyramidTopType::monument, 2, 4, 4, godI, 1}
        };
    } break;
    case eBuildingType::pyramidOfThePantheon: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 1, 1, 0},
            {eOrientation::topRight, 0, 2, 1, 0},
            {eOrientation::topRight, 0, 3, 1, 0},
            {eOrientation::topRight, 0, 4, 1, 0},
            {eOrientation::topRight, 0, 5, 1, 0},
            {eOrientation::topRight, 0, 6, 1, 0},
            {eOrientation::right, 0, 7, 1, 0},
            {eOrientation::bottomRight, 0, 7, 2, 2},
            {eOrientation::bottomRight, 0, 7, 3, 0},
            {eOrientation::bottomRight, 0, 7, 4, 0},
            {eOrientation::bottomRight, 0, 7, 5, 0},
            {eOrientation::bottomRight, 0, 7, 6, 0},
            {eOrientation::bottomRight, 0, 7, 7, 0},
            {eOrientation::bottomRight, 0, 7, 8, 2},
            {eOrientation::bottom, 0, 7, 9, 0},
            {eOrientation::bottomLeft, 0, 6, 9, 0},
            {eOrientation::bottomLeft, 0, 5, 9, 0},
            {eOrientation::bottomLeft, 0, 4, 9, 0},
            {eOrientation::bottomLeft, 0, 3, 9, 0},
            {eOrientation::bottomLeft, 0, 2, 9, 0},
            {eOrientation::left, 0, 1, 9, 0},
            {eOrientation::topLeft, 0, 1, 8, 2},
            {eOrientation::topLeft, 0, 1, 7, 0},
            {eOrientation::topLeft, 0, 1, 6, 0},
            {eOrientation::topLeft, 0, 1, 5, 0},
            {eOrientation::topLeft, 0, 1, 4, 0},
            {eOrientation::topLeft, 0, 1, 3, 0},
            {eOrientation::topLeft, 0, 1, 2, 2},

            {eOrientation::top, 1, 2, 3, 0},
            {eOrientation::topRight, 1, 3, 3, 0},
            {eOrientation::topRight, 1, 4, 3, 1},
            {eOrientation::topRight, 1, 5, 3, 0},
            {eOrientation::right, 1, 6, 3, 0},
            {eOrientation::bottomRight, 1, 6, 4, 0},
            {eOrientation::bottomRight, 1, 6, 5, 1},
            {eOrientation::bottomRight, 1, 6, 6, 0},
            {eOrientation::bottom, 1, 6, 7, 0},
            {eOrientation::bottomLeft, 1, 5, 7, 0},
            {eOrientation::bottomLeft, 1, 4, 7, 1},
            {eOrientation::bottomLeft, 1, 3, 7, 0},
            {eOrientation::left, 1, 2, 7, 0},
            {eOrientation::topLeft, 1, 2, 6, 0},
            {eOrientation::topLeft, 1, 2, 5, 1},
            {eOrientation::topLeft, 1, 2, 4, 0}
        };

        top = {
            {ePyramidTopType::statue, 0, 2, 0, 0, 0},
            {ePyramidTopType::statue, 0, 4, 0, 5, 0},
            {ePyramidTopType::statue, 0, 6, 0, 9, 0},

            {ePyramidTopType::statue, 0, 8, 3, 1, 1},
            {ePyramidTopType::statue, 0, 8, 5, 8, 1},
            {ePyramidTopType::statue, 0, 8, 7, 11, 1},

            {ePyramidTopType::statue, 0, 2, 10, 7, 2},
            {ePyramidTopType::statue, 0, 4, 10, 10, 2},
            {ePyramidTopType::statue, 0, 6, 10, 3, 2},

            {ePyramidTopType::statue, 0, 0, 3, 6, 3},
            {ePyramidTopType::statue, 0, 0, 5, 2, 3},
            {ePyramidTopType::statue, 0, 0, 7, 4, 3},

            {ePyramidTopType::tile, 0, 0, 0, 0},
            {ePyramidTopType::tile, 0, 1, 0, 0},
            {ePyramidTopType::tile, 0, 3, 0, 0},
            {ePyramidTopType::tile, 0, 5, 0, 0},
            {ePyramidTopType::tile, 0, 7, 0, 0},
            {ePyramidTopType::tile, 0, 8, 0, 0},

            {ePyramidTopType::tile, 0, 8, 1, 0},
            {ePyramidTopType::tile, 0, 8, 2, 0},
            {ePyramidTopType::tile, 0, 8, 4, 0},
            {ePyramidTopType::tile, 0, 8, 6, 0},
            {ePyramidTopType::tile, 0, 8, 8, 0},
            {ePyramidTopType::tile, 0, 8, 9, 0},
            {ePyramidTopType::tile, 0, 8, 10, 0},

            {ePyramidTopType::tile, 0, 7, 10, 0},
            {ePyramidTopType::tile, 0, 5, 10, 0},
            {ePyramidTopType::tile, 0, 3, 10, 0},
            {ePyramidTopType::tile, 0, 1, 10, 0},
            {ePyramidTopType::tile, 0, 0, 10, 0},

            {ePyramidTopType::tile, 0, 0, 9, 0},
            {ePyramidTopType::tile, 0, 0, 8, 0},
            {ePyramidTopType::tile, 0, 0, 6, 0},
            {ePyramidTopType::tile, 0, 0, 4, 0},
            {ePyramidTopType::tile, 0, 0, 2, 0},
            {ePyramidTopType::tile, 0, 0, 1, 0},

            {ePyramidTopType::tile, 1, 2, 2, 1},
            {ePyramidTopType::tile, 1, 3, 2, 0},
            {ePyramidTopType::tile, 1, 4, 2, 0},
            {ePyramidTopType::tile, 1, 5, 2, 0},
            {ePyramidTopType::tile, 1, 6, 2, 1},

            {ePyramidTopType::tile, 1, 2, 8, 1},
            {ePyramidTopType::tile, 1, 3, 8, 0},
            {ePyramidTopType::tile, 1, 4, 8, 0},
            {ePyramidTopType::tile, 1, 5, 8, 0},
            {ePyramidTopType::tile, 1, 6, 8, 1},

            {ePyramidTopType::tile, 2, 3, 4, 0},
            {ePyramidTopType::tile, 2, 5, 4, 0},

            {ePyramidTopType::tile, 2, 3, 5, 2},
            {ePyramidTopType::tile, 2, 4, 5, 0},
            {ePyramidTopType::tile, 2, 5, 5, 2},

            {ePyramidTopType::tile, 2, 3, 6, 0},
            {ePyramidTopType::tile, 2, 5, 6, 0},

            {ePyramidTopType::statue, 2, 4, 4, 12, 0},
            {ePyramidTopType::statue, 2, 4, 6, 13, 2}

        };
    } break;
    case eBuildingType::altarOfOlympus: {
        for(int i = 0; i < 3; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 2},
            {eOrientation::topRight, 0, 4, 0, 3},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::topRight, 0, 6, 0, 0},
            {eOrientation::right, 0, 7, 0, 0},
            {eOrientation::bottomRight, 0, 7, 1, 0},
            {eOrientation::bottomRight, 0, 7, 2, 0},
            {eOrientation::bottomRight, 0, 7, 3, 1},
            {eOrientation::bottomRight, 0, 7, 4, 3},
            {eOrientation::bottomRight, 0, 7, 5, 0},
            {eOrientation::bottomRight, 0, 7, 6, 0},
            {eOrientation::bottom, 0, 7, 7, 0},
            {eOrientation::bottomLeft, 0, 6, 7, 0},
            {eOrientation::bottomLeft, 0, 5, 7, 0},
            {eOrientation::bottomLeft, 0, 4, 7, 2},
            {eOrientation::bottomLeft, 0, 3, 7, 3},
            {eOrientation::bottomLeft, 0, 2, 7, 0},
            {eOrientation::bottomLeft, 0, 1, 7, 0},
            {eOrientation::left, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 1},
            {eOrientation::topLeft, 0, 0, 3, 3},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 0},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 1},
            {eOrientation::topRight, 1, 4, 1, 3},
            {eOrientation::topRight, 1, 5, 1, 0},
            {eOrientation::right, 1, 6, 1, 0},
            {eOrientation::bottomRight, 1, 6, 2, 0},
            {eOrientation::bottomRight, 1, 6, 3, 2},
            {eOrientation::bottomRight, 1, 6, 4, 3},
            {eOrientation::bottomRight, 1, 6, 5, 0},
            {eOrientation::bottom, 1, 6, 6, 0},
            {eOrientation::bottomLeft, 1, 5, 6, 0},
            {eOrientation::bottomLeft, 1, 4, 6, 1},
            {eOrientation::bottomLeft, 1, 3, 6, 3},
            {eOrientation::bottomLeft, 1, 2, 6, 0},
            {eOrientation::left, 1, 1, 6, 0},
            {eOrientation::topLeft, 1, 1, 5, 0},
            {eOrientation::topLeft, 1, 1, 4, 2},
            {eOrientation::topLeft, 1, 1, 3, 3},
            {eOrientation::topLeft, 1, 1, 2, 0},

            {eOrientation::top, 2, 2, 2, 0},
            {eOrientation::topRight, 2, 3, 2, 2},
            {eOrientation::topRight, 2, 4, 2, 3},
            {eOrientation::right, 2, 5, 2, 0},
            {eOrientation::bottomRight, 2, 5, 3, 1},
            {eOrientation::bottomRight, 2, 5, 4, 3},
            {eOrientation::bottom, 2, 5, 5, 0},
            {eOrientation::bottomLeft, 2, 4, 5, 2},
            {eOrientation::bottomLeft, 2, 3, 5, 3},
            {eOrientation::left, 2, 2, 5, 0},
            {eOrientation::topLeft, 2, 2, 4, 1},
            {eOrientation::topLeft, 2, 2, 3, 3}
        };

        top = {
            {ePyramidTopType::altar, 3, 4, 4}
        };
    } break;
    case eBuildingType::templeOfOlympus: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 1},
            {eOrientation::topRight, 0, 2, 0, 3},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 0},
            {eOrientation::topRight, 0, 5, 0, 3},
            {eOrientation::topRight, 0, 6, 0, 1},
            {eOrientation::right, 0, 7, 0, 0},
            {eOrientation::bottomRight, 0, 7, 1, 1},
            {eOrientation::bottomRight, 0, 7, 2, 3},
            {eOrientation::bottomRight, 0, 7, 3, 0},
            {eOrientation::bottomRight, 0, 7, 4, 0},
            {eOrientation::bottomRight, 0, 7, 5, 3},
            {eOrientation::bottomRight, 0, 7, 6, 1},
            {eOrientation::bottom, 0, 7, 7, 0},
            {eOrientation::bottomLeft, 0, 6, 7, 1},
            {eOrientation::bottomLeft, 0, 5, 7, 3},
            {eOrientation::bottomLeft, 0, 4, 7, 0},
            {eOrientation::bottomLeft, 0, 3, 7, 0},
            {eOrientation::bottomLeft, 0, 2, 7, 3},
            {eOrientation::bottomLeft, 0, 1, 7, 1},
            {eOrientation::left, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 1},
            {eOrientation::topLeft, 0, 0, 5, 3},
            {eOrientation::topLeft, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 3},
            {eOrientation::topLeft, 0, 0, 1, 1},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 3},
            {eOrientation::topRight, 1, 3, 1, 0},
            {eOrientation::topRight, 1, 4, 1, 0},
            {eOrientation::topRight, 1, 5, 1, 3},
            {eOrientation::right, 1, 6, 1, 0},
            {eOrientation::bottomRight, 1, 6, 2, 3},
            {eOrientation::bottomRight, 1, 6, 3, 0},
            {eOrientation::bottomRight, 1, 6, 4, 0},
            {eOrientation::bottomRight, 1, 6, 5, 3},
            {eOrientation::bottom, 1, 6, 6, 0},
            {eOrientation::bottomLeft, 1, 5, 6, 3},
            {eOrientation::bottomLeft, 1, 4, 6, 0},
            {eOrientation::bottomLeft, 1, 3, 6, 0},
            {eOrientation::bottomLeft, 1, 2, 6, 3},
            {eOrientation::left, 1, 1, 6, 0},
            {eOrientation::topLeft, 1, 1, 5, 3},
            {eOrientation::topLeft, 1, 1, 4, 0},
            {eOrientation::topLeft, 1, 1, 3, 0},
            {eOrientation::topLeft, 1, 1, 2, 3}
        };

        top = {
            {ePyramidTopType::temple, 2, 5, 5}
        };
    } break;
    case eBuildingType::observatoryKosmika: {
        for(int i = 0; i < 2; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 2},
            {eOrientation::topRight, 0, 2, 0, 0},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 3},
            {eOrientation::topRight, 0, 5, 0, 0},
            {eOrientation::topRight, 0, 6, 0, 0},
            {eOrientation::topRight, 0, 7, 0, 2},
            {eOrientation::right, 0, 8, 0, 0},
            {eOrientation::bottomRight, 0, 8, 1, 2},
            {eOrientation::bottomRight, 0, 8, 2, 0},
            {eOrientation::bottomRight, 0, 8, 3, 0},
            {eOrientation::bottomRight, 0, 8, 4, 3},
            {eOrientation::bottomRight, 0, 8, 5, 0},
            {eOrientation::bottomRight, 0, 8, 6, 0},
            {eOrientation::bottomRight, 0, 8, 7, 2},
            {eOrientation::bottom, 0, 8, 8, 0},
            {eOrientation::bottomLeft, 0, 7, 8, 2},
            {eOrientation::bottomLeft, 0, 6, 8, 0},
            {eOrientation::bottomLeft, 0, 5, 8, 0},
            {eOrientation::bottomLeft, 0, 4, 8, 3},
            {eOrientation::bottomLeft, 0, 3, 8, 0},
            {eOrientation::bottomLeft, 0, 2, 8, 0},
            {eOrientation::bottomLeft, 0, 1, 8, 2},
            {eOrientation::left, 0, 0, 8, 0},
            {eOrientation::topLeft, 0, 0, 7, 2},
            {eOrientation::topLeft, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 0},
            {eOrientation::topLeft, 0, 0, 4, 3},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 0},
            {eOrientation::topLeft, 0, 0, 1, 2},

            {eOrientation::top, 1, 1, 1, 0},
            {eOrientation::topRight, 1, 2, 1, 0},
            {eOrientation::topRight, 1, 3, 1, 1},
            {eOrientation::topRight, 1, 4, 1, 3},
            {eOrientation::topRight, 1, 5, 1, 1},
            {eOrientation::topRight, 1, 6, 1, 0},
            {eOrientation::right, 1, 7, 1, 0},
            {eOrientation::bottomRight, 1, 7, 2, 0},
            {eOrientation::bottomRight, 1, 7, 3, 1},
            {eOrientation::bottomRight, 1, 7, 4, 3},
            {eOrientation::bottomRight, 1, 7, 5, 1},
            {eOrientation::bottomRight, 1, 7, 6, 0},
            {eOrientation::bottom, 1, 7, 7, 0},
            {eOrientation::bottomLeft, 1, 6, 7, 0},
            {eOrientation::bottomLeft, 1, 5, 7, 1},
            {eOrientation::bottomLeft, 1, 4, 7, 3},
            {eOrientation::bottomLeft, 1, 3, 7, 1},
            {eOrientation::bottomLeft, 1, 2, 7, 0},
            {eOrientation::left, 1, 1, 7, 0},
            {eOrientation::topLeft, 1, 1, 6, 0},
            {eOrientation::topLeft, 1, 1, 5, 1},
            {eOrientation::topLeft, 1, 1, 4, 3},
            {eOrientation::topLeft, 1, 1, 3, 1},
            {eOrientation::topLeft, 1, 1, 2, 0}
        };

        top = {
            {ePyramidTopType::observatory, 2, 6, 6}
        };
    } break;
    case eBuildingType::museumAtlantika: {
        for(int i = 0; i < 1; i++) {
            mDark.push_back(i % 2);
        }
        walls = {
            {eOrientation::top, 0, 0, 0, 0},
            {eOrientation::topRight, 0, 1, 0, 0},
            {eOrientation::topRight, 0, 2, 0, 3},
            {eOrientation::topRight, 0, 3, 0, 0},
            {eOrientation::topRight, 0, 4, 0, 0},
            {eOrientation::topRight, 0, 5, 0, 3},
            {eOrientation::topRight, 0, 6, 0, 0},
            {eOrientation::right, 0, 7, 0, 0},
            {eOrientation::bottomRight, 0, 7, 1, 0},
            {eOrientation::bottomRight, 0, 7, 2, 3},
            {eOrientation::bottomRight, 0, 7, 3, 0},
            {eOrientation::bottomRight, 0, 7, 4, 0},
            {eOrientation::bottomRight, 0, 7, 5, 3},
            {eOrientation::bottomRight, 0, 7, 6, 0},
            {eOrientation::bottom, 0, 7, 7, 0},
            {eOrientation::bottomLeft, 0, 6, 7, 0},
            {eOrientation::bottomLeft, 0, 5, 7, 3},
            {eOrientation::bottomLeft, 0, 4, 7, 0},
            {eOrientation::bottomLeft, 0, 3, 7, 0},
            {eOrientation::bottomLeft, 0, 2, 7, 3},
            {eOrientation::bottomLeft, 0, 1, 7, 0},
            {eOrientation::left, 0, 0, 7, 0},
            {eOrientation::topLeft, 0, 0, 6, 0},
            {eOrientation::topLeft, 0, 0, 5, 3},
            {eOrientation::topLeft, 0, 0, 4, 0},
            {eOrientation::topLeft, 0, 0, 3, 0},
            {eOrientation::topLeft, 0, 0, 2, 3},
            {eOrientation::topLeft, 0, 0, 1, 0}
        };

        top = {
            {ePyramidTopType::museum, 1, 6, 6}
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
        int w;
        int h;
        stdsptr<ePyramidElement> b;
        switch(t.fType) {
        case ePyramidTopType::top: {
            b = e::make_shared<ePyramidTop>(
                    this, board, t.fElevation,
                    cid);
            w = 1;
            h = 1;
        } break;
        case ePyramidTopType::tile: {
            b = e::make_shared<ePyramidTile>(
                    this, board, t.fElevation,
                    t.fSpecial, cid);
            w = 1;
            h = 1;
        } break;
        case ePyramidTopType::altar: {
            b = e::make_shared<ePyramidAltar>(
                    this, board, t.fElevation,
                    cid);
            w = 2;
            h = 2;
        } break;
        case ePyramidTopType::statue: {
            const auto godType = static_cast<eGodType>(t.fSpecial);
            b = e::make_shared<ePyramidStatue>(
                    this, board, t.fElevation,
                    godType, t.fSpecial2, cid);
            w = 1;
            h = 1;
        } break;
        case ePyramidTopType::monument: {
            const auto godType = static_cast<eGodType>(t.fSpecial);
            b = e::make_shared<ePyramidMonument>(
                    this, board, t.fElevation,
                    godType, t.fSpecial2, cid);
            w = 2;
            h = 2;
        } break;
        case ePyramidTopType::temple: {
            b = e::make_shared<ePyramidTemple>(
                    this, board, t.fElevation, cid);
            w = 4;
            h = 4;
        } break;
        case ePyramidTopType::observatory: {
            b = e::make_shared<ePyramidObservatory>(
                    this, board, t.fElevation, cid);
            w = 5;
            h = 5;
        } break;
        case ePyramidTopType::museum: {
            b = e::make_shared<ePyramidMuseum>(
                    this, board, t.fElevation, cid);
            w = 6;
            h = 6;
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

        for(int x = 0; x < w; x++) {
            for(int y = 0; y < h; y++) {
                if(x == 0 && y == 0) continue;
                const int tx = rect.x + t.fX - x;
                const int ty = rect.y + t.fY - y;
                const auto b = e::make_shared<ePyramidBuildingPart>(
                                   this, board, t.fElevation, cid);
                const auto tile = board.tile(tx, ty);
                tile->setUnderBuilding(b);
                b->setCenterTile(tile);
                b->addUnderBuilding(tile);
                b->setTileRect({tx, ty, 1, 1});

                b->setMonument(this);
                registerElement(b);
            }
        }
    }
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
        const int level = pe->currentElevation();
        pe->setHalted(level > minLevel);
    }
}

void ePyramid::read(eReadStream& src) {
    eMonument::read(src);
    mSelf = ref<ePyramid>();
    src >> mGod;
    int ds;
    src >> ds;
    for(int i = 0; i < ds; i++) {
        bool d;
        src >> d;
        mDark.push_back(d);
    }
}

void ePyramid::write(eWriteStream& dst) const {
    eMonument::write(dst);
    dst << mGod;
    dst << mDark.size();
    for(const bool d : mDark) {
        dst << d;
    }
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

    case eBuildingType::pyramidOfThePantheon:
        sw = 9;
        sh = 11;
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
