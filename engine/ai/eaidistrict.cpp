#include "eaidistrict.h"

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

#include "buildings/eaestheticsbuilding.h"
#include "buildings/epark.h"

#include "elanguage.h"
#include "estringhelpers.h"

template <class T>
bool gBuildVendor(eGameBoard& brd,
                  eAgoraBase* const agora,
                  const int spaceId,
                  const ePlayerId pid,
                  const eCityId cid,
                  const bool editorDisplay) {
    const auto agoraP = agora->ref<eAgoraBase>();
    const auto fv = e::make_shared<T>(brd, cid);
    fv->setAgora(agoraP);
    agora->setBuilding(spaceId, fv);
    if(!editorDisplay) {
        const auto diff = brd.difficulty(pid);
        const int cost = eDifficultyHelpers::buildingCost(diff, fv->type());
        brd.incDrachmas(pid, -cost);
    }
    return true;
}

bool gBuild(const eAIBuilding& b,
            const ePlayerId pid,
            const eCityId cid,
            eGameBoard& board,
            const bool editorDisplay) {
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
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::avenue: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eAvenue>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::commonHouse: {
        const auto bc = [boardPtr, cid]() {
            return e::make_shared<eSmallHouse>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::eliteHousing: {
        const auto bc = [boardPtr, cid]() {
            return e::make_shared<eEliteHousing>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::maintenanceOffice: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eMaintenanceOffice>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::taxOffice: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eTaxOffice>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::fountain: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eFountain>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::watchPost: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eWatchpost>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::hospital: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eHospital>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::podium: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<ePodium>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::gymnasium: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eGymnasium>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::theater: {
        const auto bc = [boardPtr, cid, b]() {
            const auto bb = e::make_shared<eTheater>(*boardPtr, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::stadium: {
        const bool rotated = rect.w == 5;
        const auto bc = [boardPtr, cid, b, rotated]() {
            const auto bb = e::make_shared<eStadium>(*boardPtr, rotated, cid);
            bb->setPatrolGuides(b.fGuides);
            return bb;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
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
                    const bool cb = board.canBuild(x, y, 1, 1, false, cid, pid);
                    if(!cb) return false;
                    if(prc) prc(x, y);
                }
            }
            return true;
        };
        const auto s = e::make_shared<ePalace>(board, rotate, cid);
        forAllTiles([&](const int x, const int y) {
            board.build(x, y, 1, 1, cid, pid, false, [&]() {
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
        const bool r = board.buildBase(tminX + 1, tminY + 1, tminX + sw, tminY + sh, [&]() {
            return s;
        }, pid, cid, editorDisplay);

        board.updateMaxSoldiers(cid);
        board.distributeSoldiers(cid);
        board.consolidateSoldiers(cid);

        return r;
    } break;

    case eBuildingType::onionsFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eOnionFarm>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::carrotsFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCarrotFarm>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::wheatFarm: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eWheatFarm>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::oliveTree: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::oliveTree;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::vine: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::vine;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::orangeTree: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eResourceBuildingType::orangeTree;
            return e::make_shared<eResourceBuilding>(*boardPtr, type, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::growersLodge: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eGrowerType::grapesAndOlives;
            return e::make_shared<eGrowersLodge>(*boardPtr, type, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::orangeTendersLodge: {
        const auto bc = [boardPtr, cid, b]() {
            const auto type = eGrowerType::oranges;
            return e::make_shared<eGrowersLodge>(*boardPtr, type, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::huntingLodge: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eHuntingLodge>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::fishery: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eFishery>(*boardPtr, b.fO, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::urchinQuay: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eUrchinQuay>(*boardPtr, b.fO, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::cardingShed: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCardingShed>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::dairy: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eDairy>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::corral: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCorral>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::sheep: {
        const auto tile = board.tile(minX, minY);
        return board.buildAnimal(tile, eBuildingType::sheep,
                          [](eGameBoard& board) {
            return e::make_shared<eSheep>(board);
        }, cid, pid, editorDisplay);
    } break;
    case eBuildingType::goat: {
        const auto tile = board.tile(minX, minY);
        return board.buildAnimal(tile, eBuildingType::goat,
                          [](eGameBoard& board) {
            return e::make_shared<eGoat>(board);
        }, cid, pid, editorDisplay);
    } break;
    case eBuildingType::cattle: {
        const auto tile = board.tile(minX, minY);
        return board.buildAnimal(tile, eBuildingType::cattle,
                          [](eGameBoard& board) {
            return e::make_shared<eCattle>(board, eCharacterType::cattle2);
        }, cid, pid, editorDisplay);
    } break;

    case eBuildingType::armory: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eArmory>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::olivePress: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eOlivePress>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::winery: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eWinery>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::artisansGuild: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eArtisansGuild>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
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

        if(!editorDisplay) {
            const auto diff = board.difficulty(pid);
            const int cost = eDifficultyHelpers::buildingCost(diff, a->type());
            board.incDrachmas(pid, -cost);
        }

        gBuildVendor<eFoodVendor>(board, a.get(), 0, pid, cid, editorDisplay);
        gBuildVendor<eFleeceVendor>(board, a.get(), 1, pid, cid, editorDisplay);
        gBuildVendor<eOilVendor>(board, a.get(), 2, pid, cid, editorDisplay);
        return true;
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

        if(!editorDisplay) {
            const auto diff = board.difficulty(pid);
            const int cost = eDifficultyHelpers::buildingCost(diff, a->type());
            board.incDrachmas(pid, -cost);
        }

        gBuildVendor<eFoodVendor>(board, a.get(), 0, pid, cid, editorDisplay);
        gBuildVendor<eFleeceVendor>(board, a.get(), 1, pid, cid, editorDisplay);
        gBuildVendor<eOilVendor>(board, a.get(), 2, pid, cid, editorDisplay);
        gBuildVendor<eWineVendor>(board, a.get(), 3, pid, cid, editorDisplay);
        gBuildVendor<eArmsVendor>(board, a.get(), 4, pid, cid, editorDisplay);
        return true;
    } break;

    case eBuildingType::mint: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eMint>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::foundry: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eFoundry>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::masonryShop: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eMasonryShop>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::timberMill: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eTimberMill>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::granary: {
        const auto bc = [boardPtr, cid, b]() {
            const auto g = e::make_shared<eGranary>(*boardPtr, cid);
            g->setOrders(b.fGet, b.fEmpty, b.fAccept);
            return g;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::warehouse: {
        const auto bc = [boardPtr, cid, b]() {
            const auto w = e::make_shared<eWarehouse>(*boardPtr, cid);
            w->setOrders(b.fGet, b.fEmpty, b.fAccept);
            return w;
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::gazebo: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eGazebo>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::shellGarden: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eShellGarden>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::flowerGarden: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eFlowerGarden>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::commemorative: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eCommemorative>(0, *boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::hedgeMaze: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<eHedgeMaze>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;
    case eBuildingType::park: {
        const auto bc = [boardPtr, cid, b]() {
            return e::make_shared<ePark>(*boardPtr, cid);
        };
        return board.buildBase(minX, minY, maxX, maxY, bc, pid, cid, editorDisplay);
    } break;

    case eBuildingType::templeAphrodite:
    case eBuildingType::templeApollo:
    case eBuildingType::templeAres:
    case eBuildingType::templeArtemis:
    case eBuildingType::templeAthena:
    case eBuildingType::templeAtlas:
    case eBuildingType::templeDemeter:
    case eBuildingType::templeDionysus:
    case eBuildingType::templeHades:
    case eBuildingType::templeHephaestus:
    case eBuildingType::templeHera:
    case eBuildingType::templeHermes:
    case eBuildingType::templePoseidon:
    case eBuildingType::templeZeus: {
        const bool rotate = b.fO == eDiagonalOrientation::topLeft ||
                            b.fO == eDiagonalOrientation::bottomRight;
        return board.buildSanctuary(minX, maxX, minY, maxY, b.fType, rotate, cid, pid, editorDisplay);
    } break;
    }
    return false;
}

void eAIDistrict::build(eGameBoard& board,
                        const ePlayerId pid,
                        const eCityId cid,
                        const bool editorDisplay,
                        const eScheduleBuildingAction& scha) const {
    for(const auto& b : fBuildings) {
       const bool r = sBuild(board, pid, cid, editorDisplay, b);
       if(!r && scha) scha(b);
    }
}

bool eAIDistrict::sBuild(eGameBoard& board,
                        const ePlayerId pid,
                        const eCityId cid,
                        const bool editorDisplay,
                        const eAIBuilding& b) {
    const auto rect = b.fRect;
    const int xMin = rect.x;
    const int yMin = rect.y;
    const int xMax = rect.x + rect.w - 1;
    const int yMax = rect.y + rect.h - 1;
    {
        const auto minTile = board.tile(xMin, yMin);
        const auto minTileBT = minTile->underBuildingType();
        if(eBuilding::sSanctuaryBuilding(minTileBT)) {
            if(eBuilding::sSanctuaryBuilding(b.fType)) return false;
        } else if(minTileBT == b.fType) return false;
    }
    if(!editorDisplay) {
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
            if(!buildable) return false;
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
    }

    return gBuild(b, pid, cid, board, editorDisplay);
}

bool eAIDistrict::road(int& x, int& y) const {
    for(const auto& b : fBuildings) {
        if(b.fType == eBuildingType::road) {
            x = b.fRect.x;
            y = b.fRect.y;
            return true;
        }
    }

    return false;
}

void eAIDistrict::addBuilding(const eAIBuilding& a) {
    fBuildings.push_back(a);
}

void eAIDistrict::read(eReadStream& src) {
    int nb;
    src >> nb;
    for(int i = 0; i < nb; i++) {
        auto& b = fBuildings.emplace_back();
        b.read(src);
    }

    int nc;
    src >> nc;
    for(int i = 0; i < nc; i++) {
        auto& c = fReadyConditions.emplace_back();
        c.read(src);
    }
}

void eAIDistrict::write(eWriteStream& dst) const {
    dst << fBuildings.size();
    for(const auto& b : fBuildings) {
        b.write(dst);
    }

    dst << fReadyConditions.size();
    for(const auto& c : fReadyConditions) {
        c.write(dst);
    }
}

std::string eDistrictReadyCondition::sName(const eType type) {
    switch(type) {
    case eType::districtResourceCount:
        return eLanguage::text("district_resource_count_type");
    case eType::totalResourceCount:
        return eLanguage::text("total_resource_count_type");
    case eType::districtPopulation:
        return eLanguage::text("district_population_type");
    case eType::totalPopulation:
        return eLanguage::text("total_population_type");
    case eType::sanctuaryReady:
        return eLanguage::text("sanctuary_ready_type");
    case eType::count:
        return "";
    }
}

std::string eDistrictReadyCondition::name() const {
    std::string result;
    switch(fType) {
    case eType::districtResourceCount:
        result = eLanguage::text("district_resource_count");
        break;
    case eType::totalResourceCount:
        result = eLanguage::text("total_resource_count");
        break;
    case eType::districtPopulation:
        result = eLanguage::text("district_population");
        break;
    case eType::totalPopulation:
        result = eLanguage::text("total_population");
        break;
    case eType::sanctuaryReady:
        result = eLanguage::text("sanctuary_ready");
        break;
    case eType::count:
        return "";
    }
    switch(fType) {
    case eType::districtResourceCount:
    case eType::totalResourceCount:
        eStringHelpers::replace(result, "%1", eResourceTypeHelpers::typeName(fResource));
        eStringHelpers::replace(result, "%2", std::to_string(fValue));
        break;
    case eType::districtPopulation:
    case eType::totalPopulation:
        eStringHelpers::replace(result, "%1", std::to_string(fValue));
        break;
    case eType::sanctuaryReady:
        eStringHelpers::replace(result, "%1", eGod::sGodName(fSanctuary));
        break;
    case eType::count:
        return "";
    }
    return result;
}

void eDistrictReadyCondition::read(eReadStream& src) {
    src >> fType;
    src >> fResource;
    src >> fSanctuary;
    src >> fValue;
}

void eDistrictReadyCondition::write(eWriteStream& dst) const {
    dst << fType;
    dst << fResource;
    dst << fSanctuary;
    dst << fValue;
}
