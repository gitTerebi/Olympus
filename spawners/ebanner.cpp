#include "ebanner.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"

#include "eboarspawner.h"
#include "edeerspawner.h"
#include "elandinvasionpoint.h"
#include "eseainvasionpoint.h"
#include "edisembarkpoint.h"
#include "emonsterpoint.h"
#include "eentrypoint.h"
#include "eexitpoint.h"
#include "edisasterpoint.h"
#include "elandslidepoint.h"
#include "ewolfspawner.h"
#include "engine/etile.h"

eBanner::eBanner(const eBannerTypeS type,
                 const int id,
                 eTile* const tile,
                 GameBoard& board) :
    mType(type), mId(id), mTile(tile), mBoard(board) {
    board.registerBanner(this);
}

eBanner::~eBanner() {
    board().unregisterBanner(this);
}

GameBoard& eBanner::board() {
    if(mTile) return mTile->board();
    return mBoard;
}

eCityId eBanner::cityId() const {
    if(!mTile) return eCityId::neutralFriendly;
    return mTile->cityId();
}

bool eBanner::sBuildable(const eBannerTypeS type) {
    return type == eBannerTypeS::none ||
           type == eBannerTypeS::disasterPoint ||
           type == eBannerTypeS::landSlidePoint ||
           type == eBannerTypeS::monsterPoint;
}

bool eBanner::buildable() const {
    return sBuildable(mType);
}

void eBanner::serialize(eSaveArchive& ar) {
    ar.field("ioId", mIOID, -1);
}

eBanner* eBanner::sCreate(const int id,
                          eTile* const tile,
                          GameBoard& board,
                          const eBannerTypeS type) {
    stdsptr<eBanner> b;
    switch(type) {
    case eBannerTypeS::none:
        break;
    case eBannerTypeS::boar:
        b = std::make_shared<eBoarSpawner>(id, tile, board);
        break;
    case eBannerTypeS::deer:
        b = std::make_shared<eDeerSpawner>(id, tile, board);
        break;
    case eBannerTypeS::landInvasion:
        b = std::make_shared<eLandInvasionPoint>(id, tile, board);
        break;
    case eBannerTypeS::seaInvasion:
        b = std::make_shared<eSeaInvasionPoint>(id, tile, board);
        break;
    case eBannerTypeS::disembarkPoint:
        b = std::make_shared<eDisembarkPoint>(id, tile, board);
        break;
    case eBannerTypeS::monsterPoint:
        b = std::make_shared<eMonsterPoint>(id, tile, board);
        break;
    case eBannerTypeS::entryPoint:
        b = std::make_shared<eEntryPoint>(id, tile, board);
        break;
    case eBannerTypeS::exitPoint:
        b = std::make_shared<eExitPoint>(id, tile, board);
        break;
    case eBannerTypeS::riverEntryPoint:
        b = std::make_shared<eRiverEntryPoint>(id, tile, board);
        break;
    case eBannerTypeS::riverExitPoint:
        b = std::make_shared<eRiverExitPoint>(id, tile, board);
        break;
    case eBannerTypeS::disasterPoint:
        b = std::make_shared<eDisasterPoint>(id, tile, board);
        break;
    case eBannerTypeS::landSlidePoint:
        b = std::make_shared<eLandSlidePoint>(id, tile, board);
        break;
    case eBannerTypeS::wolf:
        b = std::make_shared<eWolfSpawner>(id, tile, board);
        break;
    }
    tile->addBanner(b);
    return b.get();
}
