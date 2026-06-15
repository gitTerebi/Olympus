#include "build-validity.h"

#include "engine/game-board.h"
#include "engine/etile.h"
#include "engine/etilebase.h"
#include "engine/eknownendpathfinder.h"
#include "buildable-helpers.h"
#include "buildings/eagoraspace.h"
#include "buildings/eagorabase.h"
#include "spawners/banner.h"

namespace BuildValidity {

namespace {

bool tileBuildable(eTile* const t) {
    if(!t) return false;
    if(t->underBuilding()) return false;
    const auto& banners = t->banners();
    for(const auto& b : banners) {
        if(!b->buildable()) return false;
    }
    if(t->isElevationTile()) return false;
    const auto& chars = t->characters();
    if(!chars.empty()) return false;
    return true;
}

} // namespace

bool canBuildVendor(GameBoard* const board, const int tx, const int ty,
                    const eResourceType resType) {
    if(!board) return false;
    const auto t = board->tile(tx, ty);
    if(!t) return false;
    const auto b = t->underBuilding();
    if(!b) return false;
    const auto bt = b->type();
    if(bt != eBuildingType::agoraSpace) return false;
    const auto space = static_cast<eAgoraSpace*>(b);
    const auto agora = space->agora();
    if(agora->vendor(resType)) return false;
    const auto ct = b->centerTile();
    if(!ct) return false;
    return ct->x() == tx && ct->y() == ty;
}

bool waterTileHasAccessToSea(GameBoard* const board,
                             const eCityId viewedCity,
                             const int tx, const int ty) {
    if(!board) return false;
    const auto t = board->tile(tx, ty);
    if(!t) return false;
    if(!t->hasWater()) return false;
    const auto riverEntry = board->riverEntryPoint(viewedCity);
    if(!riverEntry) return false;
    eKnownEndPathFinder p([](eTileBase* const tile) {
        return tile->hasWater();
    }, riverEntry);
    const int w = board->width();
    const int h = board->height();
    return p.findPath({0, 0, w, h}, t, 1000, true, w, h);
}

bool canBuildFishery(GameBoard* const board, const int tx, const int ty,
                     eDiagonalOrientation& o) {
    if(!board) return false;
    for(int x = tx; x < tx + 2; x++) {
        for(int y = ty - 1; y < ty - 1 + 2; y++) {
            const auto t = board->tile(x, y);
            if(!tileBuildable(t)) return false;
        }
    }
    const auto t = board->tile(tx, ty);
    if(!t) return false;
    if(BuildableHelpers::canBuildFisheryTR(t)) {
        o = eDiagonalOrientation::topRight;
        return true;
    }
    if(BuildableHelpers::canBuildFisheryBR(t)) {
        o = eDiagonalOrientation::bottomRight;
        return true;
    }
    if(BuildableHelpers::canBuildFisheryBL(t)) {
        o = eDiagonalOrientation::bottomLeft;
        return true;
    }
    if(BuildableHelpers::canBuildFisheryTL(t)) {
        o = eDiagonalOrientation::topLeft;
        return true;
    }
    return false;
}

bool canBuildTriremeWharf(GameBoard* const board, const int tx, const int ty,
                          eDiagonalOrientation& o) {
    if(!board) return false;
    for(int x = tx - 1; x < tx - 1 + 3; x++) {
        for(int y = ty - 1; y < ty - 1 + 3; y++) {
            const auto t = board->tile(x, y);
            if(!tileBuildable(t)) return false;
        }
    }
    {
        const auto t = board->tile(tx - 1, ty);
        if(!t) return false;
        if(BuildableHelpers::canBuildFisheryTR(t)) {
            const auto br = t->bottomRight<eTile>();
            if(BuildableHelpers::canBuildFisheryTR(br)) {
                o = eDiagonalOrientation::topRight;
                return true;
            }
        }
    }
    {
        const auto t = board->tile(tx, ty);
        if(!t) return false;
        if(BuildableHelpers::canBuildFisheryBR(t)) {
            const auto bl = t->bottomLeft<eTile>();
            if(BuildableHelpers::canBuildFisheryBR(bl)) {
                o = eDiagonalOrientation::bottomRight;
                return true;
            }
        }
    }
    {
        const auto t = board->tile(tx - 1, ty + 1);
        if(!t) return false;
        if(BuildableHelpers::canBuildFisheryBL(t)) {
            const auto br = t->bottomRight<eTile>();
            if(BuildableHelpers::canBuildFisheryBL(br)) {
                o = eDiagonalOrientation::bottomLeft;
                return true;
            }
        }
    }
    {
        const auto t = board->tile(tx - 1, ty + 1);
        if(!t) return false;
        if(BuildableHelpers::canBuildFisheryTL(t)) {
            const auto tr = t->topRight<eTile>();
            if(BuildableHelpers::canBuildFisheryTL(tr)) {
                o = eDiagonalOrientation::topLeft;
                return true;
            }
        }
    }
    return false;
}

bool canBuildPier(GameBoard* const board, const int tx, const int ty,
                  eDiagonalOrientation& o, const eCityId cid,
                  const ePlayerId pid, const bool forestAllowed) {
    if(!board) return false;
    if(!canBuildFishery(board, tx, ty, o)) return false;
    int minX;
    int minY;
    switch(o) {
    case eDiagonalOrientation::topRight:
        minX = tx - 1; minY = ty + 1; break;
    case eDiagonalOrientation::bottomRight:
        minX = tx - 4; minY = ty - 2; break;
    case eDiagonalOrientation::bottomLeft:
        minX = tx - 1; minY = ty - 5; break;
    default:
    case eDiagonalOrientation::topLeft:
        minX = tx + 2; minY = ty - 2; break;
    }
    return board->canBuildBase(minX, minX + 4, minY, minY + 4,
                                forestAllowed, cid, pid);
}

bool canBuildAvenue(GameBoard* const board, eTile* const t, const eCityId cid,
                    const ePlayerId pid, const bool forestAllowed) {
    if(!board) return false;
    return board->canBuildAvenue(t, cid, pid, forestAllowed);
}

} // namespace BuildValidity
