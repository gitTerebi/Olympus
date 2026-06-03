#include "building-reader.h"

#include "buildings/allbuildings.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

#include <cstdio>

template <typename T>
stdsptr<eBuilding> createVendorTagged(GameBoard& board,
                                     eSaveArchive&,
                                     const eCityId cid) {
    return e::make_shared<T>(board, cid);
}

stdsptr<eBuilding> BuildingArchive::load(
        GameBoard& board, const eBuildingType type,
        eSaveArchive& ar) {
    stdsptr<eBuilding> b;
    if(type == eBuildingType::palace) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            bool rotated;
            it.field("cityId", cid);
            it.field("rotated", rotated);
            b = e::make_shared<ePalace>(board, rotated, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::palaceTile) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            bool other;
            it.field("cityId", cid);
            it.field("other", other);
            b = e::make_shared<ePalaceTile>(board, other, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    {
        auto makeSimple = [&](GameBoard& brd, eCityId cid) -> stdsptr<eBuilding> {
            switch(type) {
            case eBuildingType::commonHouse: return e::make_shared<SmallHouse>(brd, cid);
            case eBuildingType::eliteHousing: return e::make_shared<EliteHousing>(brd, cid);
            case eBuildingType::road: return e::make_shared<eRoad>(brd, cid);
            case eBuildingType::roadblock: return e::make_shared<eRoad>(brd, cid);
            case eBuildingType::gymnasium: return e::make_shared<eGymnasium>(brd, cid);
            case eBuildingType::podium: return e::make_shared<ePodium>(brd, cid);
            case eBuildingType::fountain: return e::make_shared<eFountain>(brd, cid);
            case eBuildingType::watchPost: return e::make_shared<eWatchpost>(brd, cid);
            case eBuildingType::maintenanceOffice: return e::make_shared<eMaintenanceOffice>(brd, cid);
            case eBuildingType::college: return e::make_shared<eCollege>(brd, cid);
            case eBuildingType::dramaSchool: return e::make_shared<eDramaSchool>(brd, cid);
            case eBuildingType::theater: return e::make_shared<eTheater>(brd, cid);
            case eBuildingType::hospital: return e::make_shared<eHospital>(brd, cid);
            case eBuildingType::bibliotheke: return e::make_shared<eBibliotheke>(brd, cid);
            case eBuildingType::observatory: return e::make_shared<eObservatory>(brd, cid);
            case eBuildingType::university: return e::make_shared<eUniversity>(brd, cid);
            case eBuildingType::laboratory: return e::make_shared<eLaboratory>(brd, cid);
            case eBuildingType::inventorsWorkshop: return e::make_shared<eInventorsWorkshop>(brd, cid);
            case eBuildingType::museum: return e::make_shared<eMuseum>(brd, cid);
            case eBuildingType::taxOffice: return e::make_shared<eTaxOffice>(brd, cid);
            case eBuildingType::mint: return e::make_shared<eMint>(brd, cid);
            case eBuildingType::foundry: return e::make_shared<eFoundry>(brd, cid);
            case eBuildingType::refinery: return e::make_shared<eRefinery>(brd, cid);
            case eBuildingType::blackMarbleWorkshop: return e::make_shared<eBlackMarbleWorkshop>(brd, cid);
            case eBuildingType::timberMill: return e::make_shared<eTimberMill>(brd, cid);
            case eBuildingType::masonryShop: return e::make_shared<eMasonryShop>(brd, cid);
            case eBuildingType::huntingLodge: return e::make_shared<eHuntingLodge>(brd, cid);
            case eBuildingType::corral: return e::make_shared<eCorral>(brd, cid);
            case eBuildingType::dairy: return e::make_shared<eDairy>(brd, cid);
            case eBuildingType::cardingShed: return e::make_shared<eCardingShed>(brd, cid);
            case eBuildingType::wheatFarm: return e::make_shared<eWheatFarm>(brd, cid);
            case eBuildingType::onionsFarm: return e::make_shared<eOnionFarm>(brd, cid);
            case eBuildingType::carrotsFarm: return e::make_shared<eCarrotFarm>(brd, cid);
            case eBuildingType::granary: return e::make_shared<eGranary>(brd, cid);
            case eBuildingType::warehouse: return e::make_shared<Warehouse>(brd, cid);
            case eBuildingType::wall: return e::make_shared<eWall>(brd, cid);
            case eBuildingType::tower: return e::make_shared<eTower>(brd, cid);
            case eBuildingType::armory: return e::make_shared<eArmory>(brd, cid);
            case eBuildingType::olivePress: return e::make_shared<eOlivePress>(brd, cid);
            case eBuildingType::winery: return e::make_shared<eWinery>(brd, cid);
            case eBuildingType::sculptureStudio: return e::make_shared<eSculptureStudio>(brd, cid);
            case eBuildingType::artisansGuild: return e::make_shared<eArtisansGuild>(brd, cid);
            case eBuildingType::chariotFactory: return e::make_shared<eChariotFactory>(brd, cid);
            case eBuildingType::park: return e::make_shared<ePark>(brd, cid);
            case eBuildingType::doricColumn: return e::make_shared<eDoricColumn>(brd, cid);
            case eBuildingType::ionicColumn: return e::make_shared<eIonicColumn>(brd, cid);
            case eBuildingType::corinthianColumn: return e::make_shared<eCorinthianColumn>(brd, cid);
            case eBuildingType::avenue: return e::make_shared<eAvenue>(brd, cid);
            case eBuildingType::bench: return e::make_shared<eBench>(brd, cid);
            case eBuildingType::flowerGarden: return e::make_shared<eFlowerGarden>(brd, cid);
            case eBuildingType::gazebo: return e::make_shared<eGazebo>(brd, cid);
            case eBuildingType::hedgeMaze: return e::make_shared<eHedgeMaze>(brd, cid);
            case eBuildingType::fishPond: return e::make_shared<eFishPond>(brd, cid);
            case eBuildingType::waterPark: return e::make_shared<eWaterPark>(brd, cid);
            case eBuildingType::birdBath: return e::make_shared<eBirdBath>(brd, cid);
            case eBuildingType::shortObelisk: return e::make_shared<eShortObelisk>(brd, cid);
            case eBuildingType::tallObelisk: return e::make_shared<eTallObelisk>(brd, cid);
            case eBuildingType::shellGarden: return e::make_shared<eShellGarden>(brd, cid);
            case eBuildingType::sundial: return e::make_shared<eSundial>(brd, cid);
            case eBuildingType::dolphinSculpture: return e::make_shared<eDolphinSculpture>(brd, cid);
            case eBuildingType::spring: return e::make_shared<eSpring>(brd, cid);
            case eBuildingType::orrery: return e::make_shared<eOrrery>(brd, cid);
            case eBuildingType::topiary: return e::make_shared<eTopiary>(brd, cid);
            case eBuildingType::baths: return e::make_shared<eBaths>(brd, cid);
            case eBuildingType::stoneCircle: return e::make_shared<eStoneCircle>(brd, cid);
            case eBuildingType::oliveTree: return e::make_shared<eResourceBuilding>(brd, eResourceBuildingType::oliveTree, cid);
            case eBuildingType::vine: return e::make_shared<eResourceBuilding>(brd, eResourceBuildingType::vine, cid);
            case eBuildingType::orangeTree: return e::make_shared<eResourceBuilding>(brd, eResourceBuildingType::orangeTree, cid);
            case eBuildingType::sheep: return e::make_shared<eAnimalBuilding>(brd, nullptr, eBuildingType::sheep, cid);
            case eBuildingType::goat: return e::make_shared<eAnimalBuilding>(brd, nullptr, eBuildingType::goat, cid);
            case eBuildingType::cattle: return e::make_shared<eAnimalBuilding>(brd, nullptr, eBuildingType::cattle, cid);
            case eBuildingType::growersLodge: return e::make_shared<eGrowersLodge>(brd, eGrowerType::grapesAndOlives, cid);
            case eBuildingType::orangeTendersLodge: return e::make_shared<eGrowersLodge>(brd, eGrowerType::oranges, cid);
            default: return nullptr;
            }
        };
        bool simple = false;
        switch(type) {
        case eBuildingType::commonHouse:
        case eBuildingType::eliteHousing:
        case eBuildingType::road:
        case eBuildingType::roadblock:
        case eBuildingType::gymnasium:
        case eBuildingType::podium:
        case eBuildingType::fountain:
        case eBuildingType::watchPost:
        case eBuildingType::maintenanceOffice:
        case eBuildingType::college:
        case eBuildingType::dramaSchool:
        case eBuildingType::theater:
        case eBuildingType::hospital:
        case eBuildingType::bibliotheke:
        case eBuildingType::observatory:
        case eBuildingType::university:
        case eBuildingType::laboratory:
        case eBuildingType::inventorsWorkshop:
        case eBuildingType::museum:
        case eBuildingType::taxOffice:
        case eBuildingType::mint:
        case eBuildingType::foundry:
        case eBuildingType::refinery:
        case eBuildingType::blackMarbleWorkshop:
        case eBuildingType::timberMill:
        case eBuildingType::masonryShop:
        case eBuildingType::huntingLodge:
        case eBuildingType::corral:
        case eBuildingType::dairy:
        case eBuildingType::cardingShed:
        case eBuildingType::wheatFarm:
        case eBuildingType::onionsFarm:
        case eBuildingType::carrotsFarm:
        case eBuildingType::granary:
        case eBuildingType::warehouse:
        case eBuildingType::wall:
        case eBuildingType::tower:
        case eBuildingType::armory:
        case eBuildingType::olivePress:
        case eBuildingType::winery:
        case eBuildingType::sculptureStudio:
        case eBuildingType::artisansGuild:
        case eBuildingType::chariotFactory:
        case eBuildingType::park:
        case eBuildingType::doricColumn:
        case eBuildingType::ionicColumn:
        case eBuildingType::corinthianColumn:
        case eBuildingType::avenue:
        case eBuildingType::bench:
        case eBuildingType::flowerGarden:
        case eBuildingType::gazebo:
        case eBuildingType::hedgeMaze:
        case eBuildingType::fishPond:
        case eBuildingType::waterPark:
        case eBuildingType::birdBath:
        case eBuildingType::shortObelisk:
        case eBuildingType::tallObelisk:
        case eBuildingType::shellGarden:
        case eBuildingType::sundial:
        case eBuildingType::dolphinSculpture:
        case eBuildingType::spring:
        case eBuildingType::orrery:
        case eBuildingType::topiary:
        case eBuildingType::baths:
        case eBuildingType::stoneCircle:
        case eBuildingType::oliveTree:
        case eBuildingType::vine:
        case eBuildingType::orangeTree:
        case eBuildingType::sheep:
        case eBuildingType::goat:
        case eBuildingType::cattle:
        case eBuildingType::growersLodge:
        case eBuildingType::orangeTendersLodge:
            simple = true;
            break;
        default:
            break;
        }
        if(simple) {
            ar.archiveField("factory", [&](eSaveArchive& it) {
                eCityId cid;
                it.field("cityId", cid);
                b = makeSimple(board, cid);
            });
            ar.archiveField("state", [&](eSaveArchive& it) {
                if(b) b->serialize(it);
            });
            return b;
        }
    }
    if(type == eBuildingType::stadium) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            bool rotated;
            it.field("cityId", cid);
            it.field("rotated", rotated);
            b = e::make_shared<eStadium>(board, rotated, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::gatehouse) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            bool rotated;
            it.field("cityId", cid);
            it.field("rotated", rotated);
            b = e::make_shared<eGatehouse>(board, rotated, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::pier) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eDiagonalOrientation o;
            it.field("cityId", cid);
            it.field("orientation", o);
            b = e::make_shared<ePier>(board, o, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::tradePost) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            const auto& wrld = board.world();
            eCityId cid;
            eTradePostType tpt;
            eDiagonalOrientation o;
            eCityId ctid;
            it.field("cityId", cid);
            it.field("tpType", tpt);
            it.field("orientation", o);
            it.field("worldCityId", ctid);
            const auto ct = wrld.cityWithId(ctid);
            const auto tp = e::make_shared<TradePost>(board, *ct, cid, tpt);
            b = tp;
            tp->setOrientation(o);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::horseRanch) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eHorseRanch>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::horseRanchEnclosure) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<HorseRanchEnclosure>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::foodVendor ||
       type == eBuildingType::fleeceVendor ||
       type == eBuildingType::oilVendor ||
       type == eBuildingType::wineVendor ||
       type == eBuildingType::armsVendor ||
       type == eBuildingType::horseTrainer ||
       type == eBuildingType::chariotVendor) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            switch(type) {
            case eBuildingType::foodVendor: b = createVendorTagged<eFoodVendor>(board, it, cid); break;
            case eBuildingType::fleeceVendor: b = createVendorTagged<eFleeceVendor>(board, it, cid); break;
            case eBuildingType::oilVendor: b = createVendorTagged<eOilVendor>(board, it, cid); break;
            case eBuildingType::wineVendor: b = createVendorTagged<eWineVendor>(board, it, cid); break;
            case eBuildingType::armsVendor: b = createVendorTagged<eArmsVendor>(board, it, cid); break;
            case eBuildingType::horseTrainer: b = createVendorTagged<eHorseVendor>(board, it, cid); break;
            case eBuildingType::chariotVendor: b = createVendorTagged<eChariotVendor>(board, it, cid); break;
            default: break;
            }
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::urchinQuay ||
       type == eBuildingType::fishery ||
       type == eBuildingType::triremeWharf) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eDiagonalOrientation o;
            it.field("cityId", cid);
            it.field("orientation", o);
            switch(type) {
            case eBuildingType::urchinQuay:
                b = e::make_shared<eUrchinQuay>(board, o, cid); break;
            case eBuildingType::fishery:
                b = e::make_shared<eFishery>(board, o, cid); break;
            case eBuildingType::triremeWharf:
                b = e::make_shared<eTriremeWharf>(board, o, cid); break;
            default: break;
            }
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::commonAgora ||
       type == eBuildingType::grandAgora) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eAgoraOrientation o;
            it.field("cityId", cid);
            it.field("orientation", o);
            switch(type) {
            case eBuildingType::commonAgora:
                b = e::make_shared<eCommonAgora>(o, board, cid); break;
            case eBuildingType::grandAgora:
                b = e::make_shared<eGrandAgora>(o, board, cid); break;
            default: break;
            }
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::agoraSpace) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            (void)cid;
        });
        return b;
    }
    if(type == eBuildingType::commemorative) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            int id;
            it.field("cityId", cid);
            it.field("id", id);
            b = e::make_shared<eCommemorative>(id, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::godMonument) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eGodType gtype;
            eGodQuestId qid;
            it.field("cityId", cid);
            it.field("god", gtype);
            it.field("questId", qid);
            b = e::make_shared<eGodMonument>(gtype, qid, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(eBuilding::sSanctuaryBuilding(type) &&
       type != eBuildingType::templeStatue &&
       type != eBuildingType::templeMonument &&
       type != eBuildingType::templeAltar &&
       type != eBuildingType::temple &&
       type != eBuildingType::templeTile) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            int sw;
            int sh;
            it.field("cityId", cid);
            it.field("spanW", sw);
            it.field("spanH", sh);
            b = eSanctuary::sCreate(type, sw, sh, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::templeStatue) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eGodType godType;
            int id;
            it.field("cityId", cid);
            it.field("godType", godType);
            it.field("id", id);
            b = e::make_shared<eTempleStatueBuilding>(godType, id, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::templeMonument) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            eGodType godType;
            int id;
            it.field("cityId", cid);
            it.field("godType", godType);
            it.field("id", id);
            b = e::make_shared<eTempleMonumentBuilding>(godType, id, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::templeAltar) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eTempleAltarBuilding>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::temple) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eTempleBuilding>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::templeTile) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            int id;
            it.field("cityId", cid);
            it.field("id", id);
            b = e::make_shared<eTempleTileBuilding>(id, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::godMonumentTile) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eGodMonumentTile>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(eBuilding::sHeroHall(type)) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            const auto hero = eHerosHall::sHallTypeToHeroType(type);
            b = e::make_shared<eHerosHall>(hero, board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(eBuilding::sPyramidBuilding(type) &&
       type != eBuildingType::pyramidWall &&
       type != eBuildingType::pyramidTop &&
       type != eBuildingType::pyramidTile &&
       type != eBuildingType::pyramidPart &&
       type != eBuildingType::pyramidAltar &&
       type != eBuildingType::pyramidStatue &&
       type != eBuildingType::pyramidMonument &&
       type != eBuildingType::pyramidTemple &&
       type != eBuildingType::pyramidObservatory &&
       type != eBuildingType::pyramidMuseum) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            int sw;
            int sh;
            ePyramid::sDimensions(type, sw, sh);
            b = e::make_shared<ePyramid>(board, type, sw, sh, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    {
        const bool isPyramidElem =
            type == eBuildingType::pyramidWall ||
            type == eBuildingType::pyramidTop ||
            type == eBuildingType::pyramidTile ||
            type == eBuildingType::pyramidPart ||
            type == eBuildingType::pyramidAltar ||
            type == eBuildingType::pyramidStatue ||
            type == eBuildingType::pyramidMonument ||
            type == eBuildingType::pyramidTemple ||
            type == eBuildingType::pyramidObservatory ||
            type == eBuildingType::pyramidMuseum;
        if(isPyramidElem) {
            ar.archiveField("factory", [&](eSaveArchive& it) {
                eCityId cid;
                it.field("cityId", cid);
                int elevation = 0;
                eOrientation o = eOrientation::topRight;
                int special = 0;
                int subType = 0;
                eGodType godType = eGodType::aphrodite;
                int gid = 0;
                if(type == eBuildingType::pyramidWall) {
                    it.field("orientation", o);
                    it.field("elevation", elevation);
                    it.field("special", special);
                } else if(type == eBuildingType::pyramidTile) {
                    it.field("elevation", elevation);
                    it.field("subType", subType);
                } else if(type == eBuildingType::pyramidStatue ||
                          type == eBuildingType::pyramidMonument) {
                    it.field("elevation", elevation);
                    it.field("godType", godType);
                    it.field("godId", gid);
                } else {
                    it.field("elevation", elevation);
                }
                std::vector<eSanctCost> costs;
                it.arrayField("costs", costs, [](eSaveArchive& ia, eSanctCost& c) {
                    c.serialize(ia);
                });
                stdsptr<eSanctBuilding> ts;
                switch(type) {
                case eBuildingType::pyramidWall:
                    ts = e::make_shared<ePyramidWall>(costs, board, o, elevation, special, cid); break;
                case eBuildingType::pyramidTop:
                    ts = e::make_shared<ePyramidTop>(costs, board, elevation, cid); break;
                case eBuildingType::pyramidTile:
                    ts = e::make_shared<ePyramidTile>(costs, board, elevation, subType, cid); break;
                case eBuildingType::pyramidPart:
                    ts = e::make_shared<ePyramidBuildingPart>(costs, board, elevation, cid); break;
                case eBuildingType::pyramidAltar:
                    ts = e::make_shared<ePyramidAltar>(costs, board, elevation, cid); break;
                case eBuildingType::pyramidStatue:
                    ts = e::make_shared<ePyramidStatue>(costs, board, elevation, godType, gid, cid); break;
                case eBuildingType::pyramidMonument:
                    ts = e::make_shared<ePyramidMonument>(costs, board, elevation, godType, gid, cid); break;
                case eBuildingType::pyramidTemple:
                    ts = e::make_shared<ePyramidTemple>(costs, board, elevation, cid); break;
                case eBuildingType::pyramidObservatory:
                    ts = e::make_shared<ePyramidObservatory>(costs, board, elevation, cid); break;
                case eBuildingType::pyramidMuseum:
                    ts = e::make_shared<ePyramidMuseum>(costs, board, elevation, cid); break;
                default: break;
                }
                b = ts;
            });
            ar.archiveField("state", [&](eSaveArchive& it) {
                if(b) b->serialize(it);
            });
            return b;
        }
    }
    if(type == eBuildingType::ruins) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eRuins>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::placeholder) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<ePlaceholder>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    if(type == eBuildingType::hippodromePiece) {
        ar.archiveField("factory", [&](eSaveArchive& it) {
            eCityId cid;
            it.field("cityId", cid);
            b = e::make_shared<eHippodromePiece>(board, cid);
        });
        ar.archiveField("state", [&](eSaveArchive& it) {
            if(b) b->serialize(it);
        });
        return b;
    }
    return b;
}
