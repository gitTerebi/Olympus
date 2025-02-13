#include "eaicityplan.h"

#include "engine/egameboard.h"

#include "buildings/eroad.h"
#include "buildings/eavenue.h"

#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"

#include "buildings/emaintenanceoffice.h"
#include "buildings/etaxoffice.h"
#include "buildings/efountain.h"
#include "buildings/ewatchpost.h"
#include "buildings/ehospital.h"

#include "buildings/epodium.h"
#include "buildings/egymnasium.h"
#include "buildings/etheater.h"
#include "buildings/estadium.h"

#include "buildings/epalace.h"
#include "buildings/epalacetile.h"

#include "buildings/eonionfarm.h"
#include "buildings/ecarrotfarm.h"
#include "buildings/ewheatfarm.h"

#include "buildings/eresourcebuilding.h"
#include "buildings/egrowerslodge.h"

#include "buildings/ehuntinglodge.h"
#include "buildings/efishery.h"
#include "buildings/eurchinquay.h"

#include "buildings/ecardingshed.h"
#include "buildings/edairy.h"
#include "buildings/ecorral.h"

#include "characters/esheep.h"
#include "characters/egoat.h"
#include "characters/ecattle.h"

#include "buildings/earmory.h"
#include "buildings/eolivepress.h"
#include "buildings/ewinery.h"
#include "buildings/eartisansguild.h"

#include "buildings/ecommonagora.h"
#include "buildings/egrandagora.h"

#include "buildings/eagoraspace.h"
#include "buildings/efoodvendor.h"
#include "buildings/efleecevendor.h"
#include "buildings/eoilvendor.h"
#include "buildings/ewinevendor.h"
#include "buildings/earmsvendor.h"
#include "buildings/ehorsevendor.h"
#include "buildings/echariotvendor.h"

#include "buildings/emint.h"
#include "buildings/efoundry.h"
#include "buildings/emasonryshop.h"
#include "buildings/etimbermill.h"

#include "buildings/egranary.h"
#include "buildings/ewarehouse.h"

#include "evectorhelpers.h"

#include "engine/epathfinder.h"

eAICityPlan::eAICityPlan(const ePlayerId pid,
                         const eCityId cid) :
    mPid(pid), mCid(cid) {}

void eAICityPlan::addDistrict(const eAIDistrict& a) {
    mDistricts.push_back(a);
}

void eAICityPlan::buildDistrict(eGameBoard& board, const int id) {
    const bool c = districtBuilt(id);
    if(!c) mBuiltDistrics.push_back(id);
    mDistricts[id].build(board, mPid, mCid);
    connectAllBuiltDistricts(board);
}

void eAICityPlan::buildAllDistricts(eGameBoard& board) {
    const int iMax = mDistricts.size();
    for(int i = 0; i < iMax; i++) {
        buildDistrict(board, i);
    }
    connectAllBuiltDistricts(board);
}

void eAICityPlan::rebuildDistricts(eGameBoard& board) {
    for(const int id : mBuiltDistrics) {
        mDistricts[id].build(board, mPid, mCid);
    }
}

bool eAICityPlan::connectDistricts(eGameBoard& board,
                                   const int id1, const int id2) {
    if(!districtBuilt(id1)) return false;
    if(!districtBuilt(id2)) return false;

    const auto& d1 = mDistricts[id1];
    int r1x;
    int r1y;
    const bool road1 = d1.road(r1x, r1y);
    if(!road1) return false;

    const auto& d2 = mDistricts[id2];
    int r2x;
    int r2y;
    const bool road2 = d2.road(r2x, r2y);
    if(!road2) return false;

    ePathFinder p([](eTileBase* const t) {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildableAfterClear & terr);
        if(!tr) return false;
        const auto bt = t->underBuildingType();
        const bool r = bt == eBuildingType::road ||
                       bt == eBuildingType::none;
        if(!r) return false;
        if(!t->walkableElev() && t->isElevationTile()) return false;
        return true;
    }, [&](eTileBase* const t) {
        return t->x() == r2x && t->y() == r2y;
    });
    const auto startTile = board.tile(r1x, r1y);
    const int w = board.width();
    const int h = board.height();
    const bool r = p.findPath({0, 0, w, h}, startTile, 1000, true, w, h);
    if(!r) return false;
    std::vector<eOrientation> path;
    const bool rr = p.extractPath(path);
    if(!rr) return false;
    const auto boardPtr = &board;
    const auto cid = mCid;
    eTile* t = startTile;
    const auto buildRoad = [&]() {
        const auto terr = t->terrain();
        const bool tr = static_cast<bool>(eTerrain::buildable & terr);
        if(!tr) t->setTerrain(eTerrain::dry);
        board.build(t->x(), t->y(), 1, 1,
              [boardPtr, cid]() { return e::make_shared<eRoad>(*boardPtr, cid); },
              false, true);
    };
    for(int i = path.size() - 1; i >= 0; i--) {
        if(!t) break;
        buildRoad();
        t = t->neighbour<eTile>(path[i]);
    }
    if(t) buildRoad();
    return true;
}

void eAICityPlan::connectAllBuiltDistricts(eGameBoard& board) {
    for(const int i : mBuiltDistrics) {
        for(const int j : mBuiltDistrics) {
            if(i == j) continue;
            connectDistricts(board, i, j);
        }
    }
}

bool eAICityPlan::districtBuilt(const int id) const {
    return eVectorHelpers::contains(mBuiltDistrics, id);
}

template <class T>
bool gBuildVendor(eGameBoard& brd,
                  eAgoraBase* const agora,
                  const int spaceId,
                  const ePlayerId pid,
                  const eCityId cid) {
    const auto agoraP = agora->ref<eAgoraBase>();
    const auto fv = e::make_shared<T>(brd, cid);
    fv->setAgora(agoraP);
    agora->setBuilding(spaceId, fv);
    const auto diff = brd.difficulty(pid);
    const int cost = eDifficultyHelpers::buildingCost(diff, fv->type());
    brd.incDrachmas(pid, -cost);
    return true;
}

void gBuild(const eAIBuilding& b,
            const ePlayerId pid,
            const eCityId cid,
            eGameBoard& board) {
    const auto rect = b.fRect;
    const int minX = rect.x;
    const int minY = rect.y;
    const int maxX = rect.x + rect.w - 1;
    const int maxY = rect.y + rect.h - 1;
    const auto boardPtr = &board;
    switch(b.fType) {
    case eBuildingType::road: {
        const auto bc = [boardPtr, cid, b]() {
            return  e::make_shared<eRoad>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::avenue: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eAvenue>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::commonHouse: {
        const auto bc = [boardPtr, cid]() {
            return e::make_shared<eSmallHouse>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::eliteHousing: {
        const auto bc = [boardPtr, cid]() {
            return e::make_shared<eEliteHousing>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::maintenanceOffice: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eMaintenanceOffice>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::taxOffice: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eTaxOffice>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::fountain: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eFountain>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::watchPost: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eWatchpost>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::hospital: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eHospital>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::podium: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<ePodium>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::gymnasium: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eGymnasium>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::theater: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eTheater>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::stadium: {
        const bool rotated = rect.w == 5;
        const auto bc = [boardPtr, cid, b, rotated]() {
            const auto bb = e::make_shared<eStadium>(*boardPtr, rotated, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::palace: {
        const int tminX = minX;
        const int tminY = minY;
        int sw;
        int sh;
        int tmaxX;
        int tmaxY;
        const bool rotate = rect.w == 6;
        if(rotate) {
            sw = 4;
            sh = 8;
            tmaxX = tminX + 5;
            tmaxY = tminY + 8;
        } else {
            sw = 8;
            sh = 4;
            tmaxX = tminX + 8;
            tmaxY = tminY + 5;
        }
        const auto forAllTiles = [&](const std::function<void(int, int)>& prc) {
            const SDL_Rect rect{tminX + 1, tminY + 1, sw, sh};
            for(int x = tminX; x <= tmaxX; x++) {
                for(int y = tminY; y <= tmaxY; y++) {
                    const SDL_Point pt{x, y};
                    const bool r = SDL_PointInRect(&pt, &rect);
                    if(r) continue;
                    const bool cb = board.canBuild(x, y, 1, 1);
                    if(!cb) return false;
                    if(prc) prc(x, y);
                }
            }
            return true;
        };
        const auto s = e::make_shared<ePalace>(board, rotate, cid);
        forAllTiles([&](const int x, const int y) {
            board.build(x, y, 1, 1, [&]() {
                bool other = x == tminX && y == tminY;
                if(!other) {
                    if(rotate) {
                        other = x == tmaxX && y == tminY;
                    } else {
                        other = x == tminX && y == tmaxY;
                    }
                }
                const auto t = e::make_shared<ePalaceTile>(
                                   board, other, cid);
                t->setPalace(s.get());
                s->addTile(t.get());
                return t;
            });
        });
        board.buildBase(minX, minY, maxX, maxY, [&]() {
            return s;
        }, pid);

        const auto cid = s->cityId();
        board.updateMaxSoldiers(cid);
        board.distributeSoldiers(cid);
        board.consolidateSoldiers(cid);
    } break;

    case eBuildingType::onionsFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eOnionFarm>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::carrotsFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCarrotFarm>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::wheatFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eWheatFarm>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::oliveTree: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::oliveTree;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::vine: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::vine;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::orangeTree: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::orangeTree;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::growersLodge: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eGrowerType::grapesAndOlives;
            return e::make_shared<eGrowersLodge>(*boardPtr, type, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::orangeTendersLodge: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eGrowerType::oranges;
            return e::make_shared<eGrowersLodge>(*boardPtr, type, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::huntingLodge: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eHuntingLodge>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::fishery: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eFishery>(*boardPtr, b.fO, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::urchinQuay: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eUrchinQuay>(*boardPtr, b.fO, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::cardingShed: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCardingShed>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::dairy: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eDairy>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::corral: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCorral>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::sheep: {
        const auto tile = board.tile(minX, minY);
        board.buildAnimal(tile, eBuildingType::sheep,
                          [](eGameBoard& board) {
            return e::make_shared<eSheep>(board);
        }, cid);
    } break;
    case eBuildingType::goat: {
        const auto tile = board.tile(minX, minY);
        board.buildAnimal(tile, eBuildingType::goat,
                          [](eGameBoard& board) {
            return e::make_shared<eGoat>(board);
        }, cid);
    } break;
    case eBuildingType::cattle: {
        const auto tile = board.tile(minX, minY);
        board.buildAnimal(tile, eBuildingType::cattle,
                          [](eGameBoard& board) {
            return e::make_shared<eCattle>(board, eCharacterType::cattle2);
        }, cid);
    } break;

    case eBuildingType::armory: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eArmory>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::olivePress: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eOlivePress>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::winery: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eWinery>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::artisansGuild: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eArtisansGuild>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::commonAgora: {
        const auto minTile = board.tile(minX, minY);
        const bool minHasRoad = minTile->hasRoad();
        eAgoraOrientation o;
        if(rect.w == 6) {
            if(minHasRoad) {
                o = eAgoraOrientation::bottomLeft;
            } else {
                o = eAgoraOrientation::topRight;
            }
        } else { // rect.w == 3
            if(minHasRoad) {
                o = eAgoraOrientation::bottomRight;
            } else {
                o = eAgoraOrientation::topLeft;
            }
        }
        const auto a = e::make_shared<eCommonAgora>(o, board, cid);
        int ri = 0;
        for(int x = minX; x <= maxX; x++) {
            for(int y = minY; y <= maxY; y++) {
                const auto t = board.tile(x, y);
                if(t->hasRoad()) {
                    const auto bb = t->underBuilding();
                    const auto r = static_cast<eRoad*>(bb);
                    r->setUnderAgora(a.get());
                    if(ri++ == 3) a->setCenterTile(t);
                } else {
                    a->addUnderBuilding(t);
                }
            }
        }

        a->setTileRect(rect);
        a->fillSpaces();
        a->setPatrolGuides(b.fGuides);

        const auto diff = board.difficulty(pid);
        const int cost = eDifficultyHelpers::buildingCost(diff, a->type());
        board.incDrachmas(pid, -cost);

        gBuildVendor<eFoodVendor>(board, a.get(), 0, pid, cid);
        gBuildVendor<eFleeceVendor>(board, a.get(), 1, pid, cid);
        gBuildVendor<eOilVendor>(board, a.get(), 2, pid, cid);
    } break;
    case eBuildingType::grandAgora: {
        eAgoraOrientation o;
        if(rect.w == 6) {
            o = eAgoraOrientation::bottomLeft;
        } else { // rect.w == 5
            o = eAgoraOrientation::bottomRight;
        }
        const auto a = e::make_shared<eGrandAgora>(o, board, cid);
        int ri = 0;
        for(int x = minX; x <= maxX; x++) {
            for(int y = minY; y <= maxY; y++) {
                const auto t = boardPtr->tile(x, y);
                if(t->hasRoad()) {
                    const auto bb = t->underBuilding();
                    const auto r = static_cast<eRoad*>(bb);
                    r->setUnderAgora(a.get());
                    if(ri++ == 3) a->setCenterTile(t);
                } else {
                    a->addUnderBuilding(t);
                }
            }
        }

        a->setTileRect(rect);
        a->fillSpaces();
        a->setPatrolGuides(b.fGuides);

        const auto diff = board.difficulty(pid);
        const int cost = eDifficultyHelpers::buildingCost(diff, a->type());
        board.incDrachmas(pid, -cost);

        gBuildVendor<eFoodVendor>(board, a.get(), 0, pid, cid);
        gBuildVendor<eFleeceVendor>(board, a.get(), 1, pid, cid);
        gBuildVendor<eOilVendor>(board, a.get(), 2, pid, cid);
        gBuildVendor<eWineVendor>(board, a.get(), 3, pid, cid);
        gBuildVendor<eArmsVendor>(board, a.get(), 4, pid, cid);
    } break;

    case eBuildingType::mint: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eMint>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::foundry: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eFoundry>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::masonryShop: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eMasonryShop>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::timberMill: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eTimberMill>(*boardPtr, cid);
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;

    case eBuildingType::granary: {
        const auto bc = [boardPtr, cid, b]() {
            const auto g = e::make_shared<eGranary>(*boardPtr, cid);
            g->setOrders(b.fGet, b.fEmpty, b.fAccept);
            return g;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    case eBuildingType::warehouse: {
        const auto bc = [boardPtr, cid, b]() {
            const auto w = e::make_shared<eWarehouse>(*boardPtr, cid);
            w->setOrders(b.fGet, b.fEmpty, b.fAccept);
            return w;
        };
        board.buildBase(minX, minY, maxX, maxY, bc, pid);
    } break;
    }

}

void eAIDistrict::build(eGameBoard& board,
                        const ePlayerId pid,
                        const eCityId cid) const {
    for(const auto& b : mBuildings) {
        const auto rect = b.fRect;
        const int xMin = rect.x;
        const int yMin = rect.y;
        const int xMax = rect.x + rect.w - 1;
        const int yMax = rect.y + rect.h - 1;
        {
            const auto minTile = board.tile(xMin, yMin);
            const auto minTileBT = minTile->underBuildingType();
            if(minTileBT == b.fType) continue;
        }
        if(!eBuilding::sFlatBuilding(b.fType)) {
            bool buildable = true;
            for(int x = xMin; x <= xMax; x++) {
                for(int y = yMin; y <= yMax; y++) {
                    const auto tile = board.tile(x, y);
                    const bool of = tile->onFire();
                    if(of) {
                        buildable = false;
                        break;
                    }
                    const auto& chars = tile->characters();
                    if(!chars.empty()) {
                        buildable = false;
                        break;
                    }
                }
                if(!buildable) break;
            }
            if(!buildable) continue;
        }

        int allowRoadX = __INT_MAX__;
        int allowRoadY = __INT_MAX__;

        if(b.fType == eBuildingType::commonAgora ||
           b.fType == eBuildingType::grandAgora) {
            const bool zero = b.fType == eBuildingType::commonAgora &&
                              (b.fO == eDiagonalOrientation::bottomRight ||
                               b.fO == eDiagonalOrientation::bottomLeft);
            const int x1 = rect.x;
            const int y1 = rect.y;
            const int w = rect.w;
            allowRoadX = w == 6 ? __INT_MAX__ : (zero ? x1 : (x1 + 2));
            allowRoadY = w == 6 ? (zero ? y1 : (y1 + 2)) : __INT_MAX__;
        }

        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                const auto tile = board.tile(x, y);
                const auto terr = tile->terrain();
                if(terr == eTerrain::forest ||
                   terr == eTerrain::choppedForest) {
                    tile->setTerrain(eTerrain::dry);
                }
                if(x == allowRoadX || y == allowRoadY) {
                    if(tile->hasRoad()) continue;
                }
                const auto ub = tile->underBuilding();
                if(ub) ub->erase();
            }
        }

        gBuild(b, pid, cid, board);

        if(b.fType == eBuildingType::commonHouse ||
           b.fType == eBuildingType::eliteHousing) {
            const auto& bRect = b.fRect;
            for(int x = xMin - 1; x <= xMax + 1; x++) {
                for(int y = yMin - 1; y <= yMax + 1; y++) {
                    const SDL_Point p{x, y};
                    const bool i = SDL_PointInRect(&p, &bRect);
                    if(i) continue;
                    const auto tile = board.tile(x, y);
                    if(!tile) continue;
                    const auto terr = tile->terrain();
                    if(terr == eTerrain::forest ||
                       terr == eTerrain::choppedForest) {
                        tile->setTerrain(eTerrain::dry);
                    }
                    const auto ub = tile->underBuilding();
                    if(ub) continue;
                    const bool c = board.canBuildAvenue(tile);
                    if(!c) continue;
                    board.buildBase(x, y, x, y, [&]() {
                        return e::make_shared<eAvenue>(board, cid);
                    }, pid);
                }
            }
        }
    }
}

bool eAIDistrict::road(int& x, int& y) const {
    for(const auto& b : mBuildings) {
        if(b.fType == eBuildingType::road) {
            x = b.fRect.x;
            y = b.fRect.y;
            return true;
        }
    }

    return false;
}

void eAIDistrict::addBuilding(const eAIBuilding& a) {
    mBuildings.push_back(a);
}
