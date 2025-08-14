#include "ebuildingreader.h"

#include "buildings/allbuildings.h"
#include "engine/egameboard.h"

template <typename T>
stdsptr<eBuilding> createVendor(eGameBoard& board,
                                eReadStream& src,
                                const eCityId cid) {
    const auto v = e::make_shared<T>(board, cid);
    int aid;
    src >> aid;
    int sid;
    src >> sid;
    src.addPostFunc([aid, sid, v]() {
        const auto& board = v->getBoard();
        const auto a = board.buildingWithIOID(aid);
        const auto aa = a->template ref<eAgoraBase>();
        v->setAgora(aa);
        aa->setBuilding(sid, v);
    });
    return v;
}

void readSanctBuildingMonument(
        eGameBoard& board, eReadStream& src,
        const stdsptr<eSanctBuilding>& ts) {
    src.readBuilding(&board, [ts](eBuilding* const bb) {
        const auto ss = static_cast<eMonument*>(bb);
        ts->setMonument(ss);
        ss->registerElement(ts);
    });
}

std::vector<eSanctCost> readPyramidElementCost(eReadStream& src) {
    int n;
    src >> n;
    std::vector<eSanctCost> result;
    for(int i = 0; i < n; i++) {
        auto& r = result.emplace_back();
        r.read(src);
    }
    return result;
}

stdsptr<eBuilding> eBuildingReader::sRead(
        eGameBoard& board, const eBuildingType type,
        eReadStream& src) {
    const auto& wrld = board.world();
    eCityId cid;
    src >> cid;
    stdsptr<eBuilding> b;
    switch(type) {
    case eBuildingType::road: {
        const auto r = e::make_shared<eRoad>(board, cid);
        b = r;
        bool roadblock;
        src >> roadblock;
        r->setRoadblock(roadblock);
        src.readBuilding(&board, [r](eBuilding* const bb) {
            r->setUnderAgora(static_cast<eAgoraBase*>(bb));
        });
        src.readBuilding(&board, [r](eBuilding* const bb) {
            r->setUnderGatehouse(static_cast<eGatehouse*>(bb));
        });
    } break;
    case eBuildingType::commonAgora: {
        eAgoraOrientation o;
        src >> o;
        b = e::make_shared<eCommonAgora>(o, board, cid);
    } break;
    case eBuildingType::grandAgora: {
        eAgoraOrientation o;
        src >> o;
        b = e::make_shared<eGrandAgora>(o, board, cid);
    } break;
    case eBuildingType::agoraSpace:
        return nullptr;
    case eBuildingType::commonHouse: {
        b = e::make_shared<eSmallHouse>(board, cid);
    } break;
    case eBuildingType::gymnasium: {
        b = e::make_shared<eGymnasium>(board, cid);
    } break;
    case eBuildingType::podium: {
        b = e::make_shared<ePodium>(board, cid);
    } break;
    case eBuildingType::fountain: {
        b = e::make_shared<eFountain>(board, cid);
    } break;
    case eBuildingType::watchPost: {
        b = e::make_shared<eWatchpost>(board, cid);
    } break;
    case eBuildingType::maintenanceOffice: {
        b = e::make_shared<eMaintenanceOffice>(board, cid);
    } break;
    case eBuildingType::college: {
        b = e::make_shared<eCollege>(board, cid);
    } break;
    case eBuildingType::dramaSchool: {
        b = e::make_shared<eDramaSchool>(board, cid);
    } break;
    case eBuildingType::theater: {
        b = e::make_shared<eTheater>(board, cid);
    } break;
    case eBuildingType::hospital: {
        b = e::make_shared<eHospital>(board, cid);
    } break;
    case eBuildingType::stadium: {
        bool rotated;
        src >> rotated;
        b = e::make_shared<eStadium>(board, rotated, cid);
    } break;
    case eBuildingType::bibliotheke: {
        b = e::make_shared<eBibliotheke>(board, cid);
    } break;
    case eBuildingType::observatory: {
        b = e::make_shared<eObservatory>(board, cid);
    } break;
    case eBuildingType::university: {
        b = e::make_shared<eUniversity>(board, cid);
    } break;
    case eBuildingType::laboratory: {
        b = e::make_shared<eLaboratory>(board, cid);
    } break;
    case eBuildingType::inventorsWorkshop: {
        b = e::make_shared<eInventorsWorkshop>(board, cid);
    } break;
    case eBuildingType::museum: {
        b = e::make_shared<eMuseum>(board, cid);
    } break;
    case eBuildingType::palace: {
        bool rotated;
        src >> rotated;
        b = e::make_shared<ePalace>(board, rotated, cid);
    } break;
    case eBuildingType::palaceTile: {
        bool other;
        src >> other;
        const auto pt = e::make_shared<ePalaceTile>(board, other, cid);
        b = pt;
        src.readBuilding(&board, [pt](eBuilding* const bb) {
             pt->setPalace(static_cast<ePalace*>(bb));
        });
    } break;
    case eBuildingType::eliteHousing: {
        b = e::make_shared<eEliteHousing>(board, cid);
    } break;
    case eBuildingType::taxOffice: {
        b = e::make_shared<eTaxOffice>(board, cid);
    } break;
    case eBuildingType::mint: {
        b = e::make_shared<eMint>(board, cid);
    } break;
    case eBuildingType::foundry: {
        b = e::make_shared<eFoundry>(board, cid);
    } break;
    case eBuildingType::refinery: {
        b = e::make_shared<eRefinery>(board, cid);
    } break;
    case eBuildingType::blackMarbleWorkshop: {
        b = e::make_shared<eBlackMarbleWorkshop>(board, cid);
    } break;
    case eBuildingType::timberMill: {
        b = e::make_shared<eTimberMill>(board, cid);
    } break;
    case eBuildingType::masonryShop: {
        b = e::make_shared<eMasonryShop>(board, cid);
    } break;

    case eBuildingType::oliveTree: {
        b = e::make_shared<eResourceBuilding>(
                board, eResourceBuildingType::oliveTree, cid);
    } break;
    case eBuildingType::vine: {
        b = e::make_shared<eResourceBuilding>(
                board, eResourceBuildingType::vine, cid);
    } break;
    case eBuildingType::orangeTree: {
        b = e::make_shared<eResourceBuilding>(
                board, eResourceBuildingType::orangeTree, cid);
    } break;

    case eBuildingType::huntingLodge: {
        b = e::make_shared<eHuntingLodge>(board, cid);
    } break;
    case eBuildingType::corral: {
        b = e::make_shared<eCorral>(board, cid);
    } break;

    case eBuildingType::urchinQuay: {
        eDiagonalOrientation o;
        src >> o;
        b = e::make_shared<eUrchinQuay>(board, o, cid);
    } break;
    case eBuildingType::fishery: {
        eDiagonalOrientation o;
        src >> o;
        b = e::make_shared<eFishery>(board, o, cid);
    } break;
    case eBuildingType::triremeWharf: {
        eDiagonalOrientation o;
        src >> o;
        b = e::make_shared<eTriremeWharf>(board, o, cid);
    } break;

    case eBuildingType::pier: {
        eDiagonalOrientation o;
        src >> o;
        const auto p = e::make_shared<ePier>(board, o, cid);
        b = p;
        src.readBuilding(&board, [p](eBuilding* const bb) {
             p->setTradePost(bb);
        });
    } break;

    case eBuildingType::tradePost: {
        eTradePostType tpt;
        src >> tpt;
        eDiagonalOrientation o;
        src >> o;

        eCityId ctid;
        src >> ctid;
        const auto ct = wrld.cityWithId(ctid);

        const auto tp = e::make_shared<eTradePost>(board, *ct, cid, tpt);
        b = tp;
        tp->setOrientation(o);

        if(tpt == eTradePostType::pier) {
            src.readBuilding(&board, [tp](eBuilding* const bb) {
                 tp->setUnpackBuilding(bb);
            });
        }
    } break;

    case eBuildingType::dairy: {
        b = e::make_shared<eDairy>(board, cid);
    } break;
    case eBuildingType::cardingShed: {
        b = e::make_shared<eCardingShed>(board, cid);
    } break;
    case eBuildingType::sheep: {
        b = e::make_shared<eAnimalBuilding>(
                board, nullptr, eBuildingType::sheep, cid);
    } break;
    case eBuildingType::goat: {
        b = e::make_shared<eAnimalBuilding>(
                board, nullptr, eBuildingType::goat, cid);
    } break;
    case eBuildingType::cattle: {
        b = e::make_shared<eAnimalBuilding>(
                board, nullptr, eBuildingType::cattle, cid);
    } break;
    case eBuildingType::wheatFarm: {
        b = e::make_shared<eWheatFarm>(board, cid);
    } break;
    case eBuildingType::onionsFarm: {
        b = e::make_shared<eOnionFarm>(board, cid);
    } break;
    case eBuildingType::carrotsFarm: {
        b = e::make_shared<eCarrotFarm>(board, cid);
    } break;
    case eBuildingType::growersLodge: {
        b = e::make_shared<eGrowersLodge>(
                board, eGrowerType::grapesAndOlives, cid);
    } break;
    case eBuildingType::orangeTendersLodge: {
        b = e::make_shared<eGrowersLodge>(
                board, eGrowerType::oranges, cid);
    } break;
    case eBuildingType::granary: {
        b = e::make_shared<eGranary>(board, cid);
    } break;
    case eBuildingType::warehouse: {
        b = e::make_shared<eWarehouse>(board, cid);
    } break;
    case eBuildingType::wall: {
        b = e::make_shared<eWall>(board, cid);
    } break;
    case eBuildingType::tower: {
        b = e::make_shared<eTower>(board, cid);
    } break;

    case eBuildingType::gatehouse: {
        bool rotated;
        src >> rotated;
        b = e::make_shared<eGatehouse>(board, rotated, cid);
    } break;

    case eBuildingType::armory: {
        b = e::make_shared<eArmory>(board, cid);
    } break;

    case eBuildingType::horseRanch: {
        const auto hr = e::make_shared<eHorseRanch>(board, cid);
        b = hr;

        src.readBuilding(&board, [hr](eBuilding* const bb) {
             hr->setEnclosure(static_cast<eHorseRanchEnclosure*>(bb));
        });
    } break;
    case eBuildingType::horseRanchEnclosure: {
        const auto hre = e::make_shared<eHorseRanchEnclosure>(board, cid);
        b = hre;
        src.readBuilding(&board, [hre](eBuilding* const bb) {
            hre->setRanch(static_cast<eHorseRanch*>(bb));
        });
    } break;

    case eBuildingType::olivePress: {
        b = e::make_shared<eOlivePress>(board, cid);
    } break;
    case eBuildingType::winery: {
        b = e::make_shared<eWinery>(board, cid);
    } break;
    case eBuildingType::sculptureStudio: {
        b = e::make_shared<eSculptureStudio>(board, cid);
    } break;
    case eBuildingType::artisansGuild: {
        b = e::make_shared<eArtisansGuild>(board, cid);
    } break;

    case eBuildingType::foodVendor: {
        b = createVendor<eFoodVendor>(board, src, cid);
    } break;
    case eBuildingType::fleeceVendor: {
        b = createVendor<eFleeceVendor>(board, src, cid);
    } break;
    case eBuildingType::oilVendor: {
        b = createVendor<eOilVendor>(board, src, cid);
    } break;
    case eBuildingType::wineVendor: {
        b = createVendor<eWineVendor>(board, src, cid);
    } break;
    case eBuildingType::armsVendor: {
        b = createVendor<eArmsVendor>(board, src, cid);
    } break;
    case eBuildingType::horseTrainer: {
        b = createVendor<eHorseVendor>(board, src, cid);
    } break;
    case eBuildingType::chariotVendor: {
        b = createVendor<eChariotVendor>(board, src, cid);
    } break;

    case eBuildingType::chariotFactory: {
        b = e::make_shared<eChariotFactory>(board, cid);
    } break;

    case eBuildingType::park: {
        b = e::make_shared<ePark>(board, cid);
    } break;
    case eBuildingType::doricColumn: {
        b = e::make_shared<eDoricColumn>(board, cid);
    } break;
    case eBuildingType::ionicColumn: {
        b = e::make_shared<eIonicColumn>(board, cid);
    } break;
    case eBuildingType::corinthianColumn: {
        b = e::make_shared<eCorinthianColumn>(board, cid);
    } break;
    case eBuildingType::avenue: {
        b = e::make_shared<eAvenue>(board, cid);
    } break;

    case eBuildingType::commemorative: {
        int id;
        src >> id;
        b = e::make_shared<eCommemorative>(id, board, cid);
    } break;

    case eBuildingType::godMonument: {
        eGodType type;
        src >> type;
        eGodQuestId id;
        src >> id;
        b = e::make_shared<eGodMonument>(type, id, board, cid);
    } break;
    case eBuildingType::godMonumentTile: {
        const auto pt = e::make_shared<eGodMonumentTile>(board, cid);
        b = pt;
        src.readBuilding(&board, [pt](eBuilding* const bb) {
             pt->setMonument(static_cast<eGodMonument*>(bb));
        });
    } break;

    case eBuildingType::bench: {
        b = e::make_shared<eBench>(board, cid);
    } break;
    case eBuildingType::flowerGarden: {
        b = e::make_shared<eFlowerGarden>(board, cid);
    } break;
    case eBuildingType::gazebo: {
        b = e::make_shared<eGazebo>(board, cid);
    } break;
    case eBuildingType::hedgeMaze: {
        b = e::make_shared<eHedgeMaze>(board, cid);
    } break;
    case eBuildingType::fishPond: {
        b = e::make_shared<eFishPond>(board, cid);
    } break;

    case eBuildingType::waterPark: {
        int id;
        src >> id;
        const auto wp = e::make_shared<eWaterPark>(board, cid);
        b = wp;
        wp->setId(id);
    } break;

    case eBuildingType::birdBath: {
        b = e::make_shared<eBirdBath>(board, cid);
    } break;
    case eBuildingType::shortObelisk: {
        b = e::make_shared<eShortObelisk>(board, cid);
    } break;
    case eBuildingType::tallObelisk: {
        b = e::make_shared<eTallObelisk>(board, cid);
    } break;
    case eBuildingType::shellGarden: {
        b = e::make_shared<eShellGarden>(board, cid);
    } break;
    case eBuildingType::sundial: {
        b = e::make_shared<eSundial>(board, cid);
    } break;
    case eBuildingType::dolphinSculpture: {
        b = e::make_shared<eDolphinSculpture>(board, cid);
    } break;
    case eBuildingType::spring: {
        b = e::make_shared<eSpring>(board, cid);
    } break;
    case eBuildingType::orrery: {
        b = e::make_shared<eOrrery>(board, cid);
    } break;
    case eBuildingType::topiary: {
        b = e::make_shared<eTopiary>(board, cid);
    } break;
    case eBuildingType::baths: {
        b = e::make_shared<eBaths>(board, cid);
    } break;
    case eBuildingType::stoneCircle: {
        b = e::make_shared<eStoneCircle>(board, cid);
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
        int sw;
        src >> sw;
        int sh;
        src >> sh;

        b = eSanctuary::sCreate(type, sw, sh, board, cid);
    } break;
    case eBuildingType::templeStatue: {
        eGodType godType;
        src >> godType;
        int id;
        src >> id;
        const auto ts = e::make_shared<eTempleStatueBuilding>(godType, id, board, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::templeMonument: {
        eGodType godType;
        src >> godType;
        int id;
        src >> id;
        const auto ts = e::make_shared<eTempleMonumentBuilding>(godType, id, board, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::templeAltar: {
        const auto ts = e::make_shared<eTempleAltarBuilding>(board, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::temple: {
        const auto ts = e::make_shared<eTempleBuilding>(board, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::templeTile: {
        int id;
        src >> id;
        const auto ts = e::make_shared<eTempleTileBuilding>(id, board, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;

    case eBuildingType::modestPyramid:
    case eBuildingType::pyramid:
    case eBuildingType::greatPyramid:
    case eBuildingType::majesticPyramid:

    case eBuildingType::smallMonumentToTheSky:
    case eBuildingType::monumentToTheSky:
    case eBuildingType::grandMonumentToTheSky:

    case eBuildingType::minorShrineAphrodite:
    case eBuildingType::minorShrineApollo:
    case eBuildingType::minorShrineAres:
    case eBuildingType::minorShrineArtemis:
    case eBuildingType::minorShrineAthena:
    case eBuildingType::minorShrineAtlas:
    case eBuildingType::minorShrineDemeter:
    case eBuildingType::minorShrineDionysus:
    case eBuildingType::minorShrineHades:
    case eBuildingType::minorShrineHephaestus:
    case eBuildingType::minorShrineHera:
    case eBuildingType::minorShrineHermes:
    case eBuildingType::minorShrinePoseidon:
    case eBuildingType::minorShrineZeus:

    case eBuildingType::shrineAphrodite:
    case eBuildingType::shrineApollo:
    case eBuildingType::shrineAres:
    case eBuildingType::shrineArtemis:
    case eBuildingType::shrineAthena:
    case eBuildingType::shrineAtlas:
    case eBuildingType::shrineDemeter:
    case eBuildingType::shrineDionysus:
    case eBuildingType::shrineHades:
    case eBuildingType::shrineHephaestus:
    case eBuildingType::shrineHera:
    case eBuildingType::shrineHermes:
    case eBuildingType::shrinePoseidon:
    case eBuildingType::shrineZeus:

    case eBuildingType::majorShrineAphrodite:
    case eBuildingType::majorShrineApollo:
    case eBuildingType::majorShrineAres:
    case eBuildingType::majorShrineArtemis:
    case eBuildingType::majorShrineAthena:
    case eBuildingType::majorShrineAtlas:
    case eBuildingType::majorShrineDemeter:
    case eBuildingType::majorShrineDionysus:
    case eBuildingType::majorShrineHades:
    case eBuildingType::majorShrineHephaestus:
    case eBuildingType::majorShrineHera:
    case eBuildingType::majorShrineHermes:
    case eBuildingType::majorShrinePoseidon:
    case eBuildingType::majorShrineZeus:

    case eBuildingType::pyramidOfThePantheon:
    case eBuildingType::altarOfOlympus:
    case eBuildingType::templeOfOlympus:
    case eBuildingType::observatoryKosmika:
    case eBuildingType::museumAtlantika: {
        int sw;
        int sh;
        ePyramid::sDimensions(type, sw, sh);
        b = e::make_shared<ePyramid>(board, type, sw, sh, cid);
    } break;
    case eBuildingType::pyramidWall: {
        eOrientation o;
        src >> o;
        int elevation;
        src >> elevation;
        int special;
        src >> special;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidWall>(
                            costs, board, o, elevation, special, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidTop: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidTop>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidTile: {
        int elevation;
        src >> elevation;
        int type;
        src >> type;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidTile>(
                            costs, board, elevation, type, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidPart: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidBuildingPart>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidAltar: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidAltar>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidStatue: {
        int elevation;
        src >> elevation;
        eGodType type;
        src >> type;
        int id;
        src >> id;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidStatue>(
                            costs, board, elevation, type, id, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidMonument: {
        int elevation;
        src >> elevation;
        eGodType type;
        src >> type;
        int id;
        src >> id;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidMonument>(
                            costs, board, elevation, type, id, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidTemple: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidTemple>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidObservatory: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidObservatory>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;
    case eBuildingType::pyramidMuseum: {
        int elevation;
        src >> elevation;
        const auto costs = readPyramidElementCost(src);
        const auto ts = e::make_shared<ePyramidMuseum>(
                            costs, board, elevation, cid);
        b = ts;
        readSanctBuildingMonument(board, src, ts);
    } break;

    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall: {
        const auto hero = eHerosHall::sHallTypeToHeroType(type);
        b = e::make_shared<eHerosHall>(hero, board, cid);
    } break;

    case eBuildingType::ruins: {
        b = e::make_shared<eRuins>(board, cid);
    } break;
    case eBuildingType::placeholder: {
        b = e::make_shared<ePlaceholder>(board, cid);
    } break;

    case eBuildingType::none:
    case eBuildingType::erase:
    case eBuildingType::bridge:
        return nullptr;
    }
    if(b) b->read(src);
    return b;
}
