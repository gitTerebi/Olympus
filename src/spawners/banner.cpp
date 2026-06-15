#include "banner.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"

#include "boar-spawner.h"
#include "deer-spawner.h"
#include "land-invasion-point.h"
#include "sea-invasion-point.h"
#include "disembark-point.h"
#include "monster-point.h"
#include "entry-point.h"
#include "exit-point.h"
#include "disaster-point.h"
#include "landslide-point.h"
#include "wolf-spawner.h"
#include "engine/etile.h"
#include "characters/soldier-banner.h"

Banner::Banner(const BannerTypeS type,
                 const int id,
                 eTile* const tile,
                 GameBoard& board) :
    mType(type), mId(id), mTile(tile), mBoard(board) {
    board.registerBanner(this);
}

Banner::~Banner() {
    board().unregisterBanner(this);
}

GameBoard& Banner::board() {
    if(mTile) return mTile->board();
    return mBoard;
}

eCityId Banner::cityId() const {
    if(!mTile) return eCityId::neutralFriendly;
    return mTile->cityId();
}

bool Banner::sBuildable(const BannerTypeS type) {
    return type == BannerTypeS::none ||
           type == BannerTypeS::disasterPoint ||
           type == BannerTypeS::landSlidePoint ||
           type == BannerTypeS::monsterPoint;
}

bool Banner::buildable() const {
    return sBuildable(mType);
}

void Banner::serialize(SaveArchive& ar) {
    ar.field("ioId", mIOID, -1);
}

Banner* Banner::sCreate(const int id,
                          eTile* const tile,
                          GameBoard& board,
                          const BannerTypeS type) {
    stdsptr<Banner> b;
    switch(type) {
    case BannerTypeS::none:
        break;
    case BannerTypeS::boar:
        b = std::make_shared<BoarSpawner>(id, tile, board);
        break;
    case BannerTypeS::deer:
        b = std::make_shared<DeerSpawner>(id, tile, board);
        break;
    case BannerTypeS::landInvasion:
        b = std::make_shared<LandInvasionPoint>(id, tile, board);
        break;
    case BannerTypeS::seaInvasion:
        b = std::make_shared<SeaInvasionPoint>(id, tile, board);
        break;
    case BannerTypeS::disembarkPoint:
        b = std::make_shared<DisembarkPoint>(id, tile, board);
        break;
    case BannerTypeS::monsterPoint:
        b = std::make_shared<MonsterPoint>(id, tile, board);
        break;
    case BannerTypeS::entryPoint:
        b = std::make_shared<EntryPoint>(id, tile, board);
        break;
    case BannerTypeS::exitPoint:
        b = std::make_shared<ExitPoint>(id, tile, board);
        break;
    case BannerTypeS::riverEntryPoint:
        b = std::make_shared<RiverEntryPoint>(id, tile, board);
        break;
    case BannerTypeS::riverExitPoint:
        b = std::make_shared<RiverExitPoint>(id, tile, board);
        break;
    case BannerTypeS::disasterPoint:
        b = std::make_shared<DisasterPoint>(id, tile, board);
        break;
    case BannerTypeS::landSlidePoint:
        b = std::make_shared<LandSlidePoint>(id, tile, board);
        break;
    case BannerTypeS::wolf:
        b = std::make_shared<WolfSpawner>(id, tile, board);
        break;
    }
    tile->addBanner(b);
    return b.get();
}
