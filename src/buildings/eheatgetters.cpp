#include "eheatgetters.h"
#include "engine/etilebase.h"

#include "characters/gods/egodtype.h"
#include "engine/difficulty.h"
#include "engine/model-data.h"

// Auto-fill ring fields from short {value, range} form.
// Positive value → step -1 (nice fades out), negative value → step +1 (penalty fades to 0).
// Step tiles default to 1 ring per tile (Zeus vanilla default for most).
static eHeat mk(const int v, const int r) {
    if(v == 0 || r <= 0) return {0, 1, 0, 0};
    return {v, 1, v > 0 ? -1 : 1, r};
}

static eHeat modelHeat(const char* const name, const Difficulty diff) {
    const auto stats = ModelData::instance().building(diff, name);
    if(!stats) return mk(0, 0);
    return {stats->desirability,
            stats->desirabilityStepTiles,
            stats->desirabilityStepSize,
            stats->desirabilityRange};
}

eHeat eHeatGetters::appeal(const eBuildingType type) {
    return appeal(type, Difficulty::hero);
}

eHeat eHeatGetters::appeal(const eBuildingType type, const Difficulty diff) {
    switch(type) {
    case eBuildingType::wheatFarm: return modelHeat("BUILD_WHEAT_FARM", diff);
    case eBuildingType::carrotsFarm: return modelHeat("BUILD_CARROT_FARM", diff);
    case eBuildingType::onionsFarm: return modelHeat("BUILD_ONION_FARM", diff);
    case eBuildingType::growersLodge: return modelHeat("BUILD_GROWERS_LODGE", diff);
    case eBuildingType::oliveTree:
    case eBuildingType::vine: return modelHeat("BUILD_GRAPEVINE", diff);
    case eBuildingType::orangeTree: return modelHeat("BUILD_ORANGE_TREE", diff);
    case eBuildingType::cardingShed: return modelHeat("BUILD_SHEEP_FARM", diff);
    case eBuildingType::dairy: return modelHeat("BUILD_GOAT_FARM", diff);
    case eBuildingType::urchinQuay:
    case eBuildingType::fishery: return modelHeat("BUILD_FISHING_WHARF", diff);
    case eBuildingType::huntingLodge: return modelHeat("BUILD_HUNTING_LODGE", diff);
    case eBuildingType::corral: return modelHeat("BUILD_CORRAL", diff);
    case eBuildingType::orangeTendersLodge: return modelHeat("BUILD_FRUIT_GROWERS_LODGE", diff);

    case eBuildingType::timberMill: return modelHeat("BUILD_TIMBER_MILL", diff);
    case eBuildingType::masonryShop: return modelHeat("BUILD_MARBLE_QUARRY", diff);
    case eBuildingType::blackMarbleWorkshop: return modelHeat("BUILD_BLACK_MARBLE_SHOP", diff);
    case eBuildingType::mint: return modelHeat("BUILD_MINT", diff);
    case eBuildingType::foundry: return modelHeat("BUILD_FOUNDRY", diff);
    case eBuildingType::refinery: return modelHeat("BUILD_ORICHALC_REFINERY", diff);
    case eBuildingType::sculptureStudio: return modelHeat("BUILD_SCULPTURE_STUDIO", diff);
    case eBuildingType::olivePress: return modelHeat("BUILD_OIL_PRESS", diff);
    case eBuildingType::winery: return modelHeat("BUILD_WINERY", diff);
    case eBuildingType::armory: return modelHeat("BUILD_ARMORY", diff);
    case eBuildingType::tower: return modelHeat("BUILD_TOWER", diff);

    case eBuildingType::granary: return modelHeat("BUILD_GRANARY", diff);
    case eBuildingType::warehouse: return modelHeat("BUILD_STOREHOUSE", diff);
    case eBuildingType::commonAgora:
    case eBuildingType::grandAgora: return modelHeat("BUILD_AGORA_SMALL", diff);
    case eBuildingType::tradePost: return modelHeat("BUILD_TRADING_POST", diff);
    case eBuildingType::pier: return modelHeat("BUILD_DOCK", diff);
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::wineVendor: return modelHeat("BUILD_FOOD_VENDOR", diff);
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor: return modelHeat("BUILD_HORSE_TRAINER", diff);

    case eBuildingType::horseRanch: return modelHeat("BUILD_HORSE_RANCH", diff);
    case eBuildingType::horseRanchEnclosure: return modelHeat("BUILD_HORSE_CORRAL", diff);

    case eBuildingType::fountain: return modelHeat("BUILD_FOUNTAIN", diff);
    case eBuildingType::hospital: return modelHeat("BUILD_INFIRMARY", diff);
    case eBuildingType::maintenanceOffice: return modelHeat("BUILD_SUPER_OFFICE", diff);
    case eBuildingType::watchPost: return modelHeat("BUILD_GUARDPOST", diff);
    case eBuildingType::palace: return modelHeat("BUILD_PALACE", diff);
    case eBuildingType::taxOffice: return modelHeat("BUILD_TAX_OFFICE", diff);

    case eBuildingType::college: return modelHeat("BUILD_COLLEGE", diff);
    case eBuildingType::podium: return modelHeat("BUILD_PODIUM", diff);
    case eBuildingType::gymnasium: return modelHeat("BUILD_GYMNASIUM", diff);
    case eBuildingType::dramaSchool: return modelHeat("BUILD_DRAMA_SCHOOL", diff);
    case eBuildingType::theater: return modelHeat("BUILD_THEATRE", diff);
    case eBuildingType::stadium: return modelHeat("BUILD_STADIUM", diff);

    case eBuildingType::bibliotheke: return modelHeat("BUILD_BIBLIOTHEKE", diff);
    case eBuildingType::university: return modelHeat("BUILD_UNIVERSITY", diff);
    case eBuildingType::observatory: return modelHeat("BUILD_OBSERVATORY", diff);
    case eBuildingType::museum: return modelHeat("BUILD_MUSUEM", diff);

    // Columns: 4/2/0 → step -2 per tile
    case eBuildingType::doricColumn: return modelHeat("BUILD_DORIC_COLUMN", diff);
    case eBuildingType::ionicColumn: return modelHeat("BUILD_IONIC_COLUMN", diff);
    case eBuildingType::corinthianColumn: return modelHeat("BUILD_CORINTHIAN_COLUMN", diff);

    // Avenue per-tile: vanilla anisotropic 5/3/1 along axis, 6/5/3/1 across.
    // Repo ring approximation: ring1=5, fade -2/ring, range 3 → 5/3/1.
    case eBuildingType::avenue: return modelHeat("BUILD_AVENUE", diff);

    // Commemorative: 30/30/25/25/20/20 → 2-tile rings, -5 per ring
    case eBuildingType::commemorative: return modelHeat("BUILD_COMMEMORATIVE", diff);
    case eBuildingType::godMonument: return modelHeat("BUILD_STATUE_CONAN", diff);

    case eBuildingType::park: return modelHeat("BUILD_GARDENS", diff);

    // Bench: 2/2/1/1 → 2-tile rings, -1 per ring
    case eBuildingType::bench: return modelHeat("BUILD_BENCH", diff);
    case eBuildingType::flowerGarden: return modelHeat("BUILD_FLOWER_GARDEN", diff);
    case eBuildingType::gazebo: return modelHeat("BUILD_GAZEBO", diff);
    case eBuildingType::hedgeMaze: return modelHeat("BUILD_HEDGE_MAZE", diff);
    // Fish Pond: 18/15/12/9/6/3 → step -3
    case eBuildingType::fishPond: return modelHeat("BUILD_FISH_POND", diff);

    case eBuildingType::waterPark: return modelHeat("BUILD_WATER_PARK", diff);

    case eBuildingType::birdBath: return modelHeat("BUILD_BIRD_BATH", diff);
    case eBuildingType::shortObelisk: return modelHeat("BUILD_SHORT_OBELISK", diff);
    // Tall Obelisk: 8/8 flat → 2-tile rings, step 0
    case eBuildingType::tallObelisk: return modelHeat("BUILD_TALL_OBELISK", diff);
    case eBuildingType::shellGarden: return modelHeat("BUILD_SHELL_GARDEN", diff);
    // Sundial: 7/5/3/1 → step -2
    case eBuildingType::sundial: return modelHeat("BUILD_SUNDIAL", diff);
    // Dolphin: 12/12/9/9 → 2-tile rings, -3
    case eBuildingType::dolphinSculpture: return modelHeat("BUILD_DOLPHIN_SCULPTURE", diff);
    case eBuildingType::orrery: return modelHeat("BUILD_ORRERY", diff);
    // Spring: 10/10/9/9/8 → 2-tile rings, -1
    case eBuildingType::spring: return modelHeat("BUILD_SPRING", diff);
    // Topiary: 15 flat for 3 tiles
    case eBuildingType::topiary: return modelHeat("BUILD_TOPIARY", diff);
    case eBuildingType::baths: return modelHeat("BUILD_BATHS", diff);
    // Stone Circle: 24/24/22/22 → 2-tile rings, -2
    case eBuildingType::stoneCircle: return modelHeat("BUILD_STONE_CIRCLE", diff);

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
    case eBuildingType::templeZeus: return modelHeat("BUILD_LARGE_SANC_ZEUS", diff);

    case eBuildingType::modestPyramid: return modelHeat("BUILD_POSEIDON_PYRAMID1", diff);
    case eBuildingType::pyramid: return modelHeat("BUILD_POSEIDON_PYRAMID2", diff);
    case eBuildingType::greatPyramid: return modelHeat("BUILD_POSEIDON_PYRAMID3", diff);
    case eBuildingType::majesticPyramid: return modelHeat("BUILD_POSEIDON_PYRAMID4", diff);

    case eBuildingType::smallMonumentToTheSky: return modelHeat("BUILD_POSEIDON_PYRAMID5", diff);
    case eBuildingType::monumentToTheSky: return modelHeat("BUILD_POSEIDON_PYRAMID6", diff);
    case eBuildingType::grandMonumentToTheSky: return modelHeat("BUILD_POSEIDON_PYRAMID7", diff);

    case eBuildingType::minorShrineAphrodite: // 3x3
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
    case eBuildingType::minorShrineZeus: return modelHeat("BUILD_POSEIDON_PYRAMID8", diff);

    case eBuildingType::shrineAphrodite: // 6x6
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
    case eBuildingType::shrineZeus: return modelHeat("BUILD_POSEIDON_PYRAMID9", diff);

    case eBuildingType::majorShrineAphrodite: // 8x8
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
    case eBuildingType::majorShrineZeus: return modelHeat("BUILD_POSEIDON_PYRAMID10", diff);

    case eBuildingType::pyramidOfThePantheon: return modelHeat("BUILD_POSEIDON_PYRAMID11", diff);
    case eBuildingType::altarOfOlympus:  return modelHeat("BUILD_POSEIDON_PYRAMID12", diff);
    case eBuildingType::templeOfOlympus:  return modelHeat("BUILD_POSEIDON_PYRAMID13", diff);
    case eBuildingType::observatoryKosmika:  return modelHeat("BUILD_POSEIDON_PYRAMID14", diff);
    case eBuildingType::museumAtlantika: return modelHeat("BUILD_POSEIDON_PYRAMID15", diff);

    case eBuildingType::artisansGuild: return modelHeat("BUILD_ARTISANS_GUILD", diff);

    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall: return modelHeat("BUILD_HEROES_HALL", diff);

    case eBuildingType::triremeWharf: return modelHeat("BUILD_WARSHIP_WHARF", diff);

    case eBuildingType::chariotFactory: return modelHeat("BUILD_CHARIOT_WORKSHOP", diff);
    case eBuildingType::hippodromePiece: return modelHeat("BUILD_RACE_TRACK", diff);

    case eBuildingType::eliteHousing: return mk(0, 0);

    default: return mk(0, 0);
    }
}

eHeat eHeatGetters::housing(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return mk(0, 0);
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return mk(0, 0);
    switch(type) {
    case eBuildingType::commonHouse: return mk(10, 5);
    case eBuildingType::eliteHousing: return mk(10, 5);
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeat eHeatGetters::industry(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return mk(0, 0);
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return mk(0, 0);
    switch(type) {
    case eBuildingType::timberMill: return mk(10, 5);
    case eBuildingType::masonryShop: return mk(10, 5);
    case eBuildingType::mint: return mk(10, 5);
    case eBuildingType::foundry: return mk(10, 5);
    case eBuildingType::sculptureStudio: return mk(10, 5);
    case eBuildingType::armory: return mk(10, 5);
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeat eHeatGetters::farming(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return mk(0, 0);
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return mk(0, 0);
    switch(type) {
    case eBuildingType::wheatFarm: return mk(10, 5);
    case eBuildingType::carrotsFarm: return mk(10, 5);
    case eBuildingType::onionsFarm: return mk(10, 5);
    case eBuildingType::growersLodge: return mk(10, 5);
    case eBuildingType::cardingShed: return mk(10, 5);
    case eBuildingType::dairy: return mk(10, 5);
    case eBuildingType::urchinQuay: return mk(10, 5);
    case eBuildingType::fishery: return mk(10, 5);
    case eBuildingType::corral: return mk(10, 5);
    case eBuildingType::huntingLodge: return mk(10, 5);
    case eBuildingType::orangeTendersLodge: return mk(10, 5);
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine: return mk(2, 3);
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeat eHeatGetters::storage(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return mk(0, 0);
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return mk(0, 0);
    switch(type) {
    case eBuildingType::granary: return mk(10, 5);
    case eBuildingType::warehouse: return mk(10, 5);
    case eBuildingType::tradePost: return mk(10, 5);
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeat eHeatGetters::culture(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return mk(0, 0);
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return mk(0, 0);
    switch(type) {
    case eBuildingType::college: return mk(10, 5);
    case eBuildingType::podium: return mk(10, 5);
    case eBuildingType::gymnasium: return mk(10, 5);
    case eBuildingType::dramaSchool: return mk(10, 5);
    case eBuildingType::theater: return mk(10, 5);
    case eBuildingType::stadium: return mk(10, 5);
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeatGetters::eHeatGetter eHeatGetters::godLeaning(const eGodType gt) {
    eHeatGetter hg;
    switch(gt) {
    case eGodType::aphrodite:
        hg = eHeatGetters::housing;
        break;
    case eGodType::apollo:
        hg = eHeatGetters::culture;
        break;
    case eGodType::ares:
        hg = eHeatGetters::industry;
        break;
    case eGodType::artemis:
        hg = eHeatGetters::farming;
        break;
    case eGodType::athena:
        hg = eHeatGetters::industry;
        break;
    case eGodType::atlas:
        hg = eHeatGetters::industry;
        break;
    case eGodType::demeter:
        hg = eHeatGetters::farming;
        break;
    case eGodType::dionysus:
        hg = eHeatGetters::farming;
        break;
    case eGodType::hades:
        hg = eHeatGetters::storage;
        break;
    case eGodType::hephaestus:
        hg = eHeatGetters::industry;
        break;
    case eGodType::hera:
        hg = eHeatGetters::farming;
        break;
    case eGodType::hermes:
        hg = eHeatGetters::culture;
        break;
    case eGodType::poseidon:
        hg = eHeatGetters::farming;
        break;
    case eGodType::zeus:
        hg = eHeatGetters::storage;
        break;
    }
    return hg;
}

eHeat eHeatGetters::empty(const eBuildingType type) {
    (void)type;
    switch(type) {
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(-2, 5);
    }
}

eHeat eHeatGetters::any(const eBuildingType type) {
    (void)type;
    switch(type) {
    case eBuildingType::ruins:
    case eBuildingType::placeholder:
    case eBuildingType::wall:
    case eBuildingType::gatehouse:
    case eBuildingType::tower:
    case eBuildingType::road:
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine:
    case eBuildingType::sheep:
    case eBuildingType::cattle:
    case eBuildingType::goat:
    case eBuildingType::palaceTile:
        return mk(0, 0);
    default: return mk(2, 5);
    }
}

eHeat eHeatGetters::fertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return mk(2, 8);
    } else {
        return mk(0, 0);
    }
}

eHeat eHeatGetters::notFertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return mk(0, 0);
    } else {
        return mk(2, 8);
    }
}

