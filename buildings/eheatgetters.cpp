#include "eheatgetters.h"

#include "characters/gods/egodtype.h"

eHeat eHeatGetters::appeal(const eBuildingType type) {
    switch(type) {
    case eBuildingType::wheatFarm: return {-3, 3};
    case eBuildingType::carrotsFarm: return {-4, 3};
    case eBuildingType::onionsFarm: return {-2, 2};
    case eBuildingType::growersLodge: return {-3, 2};
    case eBuildingType::oliveTree:
    case eBuildingType::vine: return {1, 1};
    case eBuildingType::orangeTree: return {1, 2};
    case eBuildingType::cardingShed: return {-5, 3};
    case eBuildingType::dairy: return {-5, 3};
    case eBuildingType::urchinQuay:
    case eBuildingType::fishery: return {-15, 4};
    case eBuildingType::huntingLodge: return {-6, 4};
    case eBuildingType::corral: return {-12, 5};
    case eBuildingType::orangeTendersLodge: return {2, 2};

    case eBuildingType::timberMill: return {-5, 4};
    case eBuildingType::masonryShop: return {-6, 4};
    case eBuildingType::blackMarbleWorkshop: return {-6, 4};
    case eBuildingType::mint:
    case eBuildingType::foundry: return {-10, 6};
    case eBuildingType::refinery: return {-10, 6};
    case eBuildingType::sculptureStudio: return {-2, 2};
    case eBuildingType::olivePress: return {-4, 4};
    case eBuildingType::winery: return {4, 4};
    case eBuildingType::armory: return {-8, 4};
    case eBuildingType::tower: return {-4, 4};

    case eBuildingType::granary: return {-12, 4};
    case eBuildingType::warehouse: return {-2, 4};
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::wineVendor: return {2, 2};
    case eBuildingType::horseTrainer:
    case eBuildingType::chariotVendor: return {-2, 2};

    case eBuildingType::horseRanch:
    case eBuildingType::horseRanchEnclosure: return {-10, 4};

    case eBuildingType::fountain: return {4, 4};
    case eBuildingType::hospital: return {-2, 4};
    case eBuildingType::maintenanceOffice: return {0, 0};
    case eBuildingType::watchPost: return {-2, 2};
    case eBuildingType::palace: return {18, 6};
    case eBuildingType::taxOffice: return {-4, 2};

    case eBuildingType::college: return {-5, 2};
    case eBuildingType::podium: return {3, 3};
    case eBuildingType::gymnasium: return {0, 0};
    case eBuildingType::dramaSchool: return {0, 0};
    case eBuildingType::theater: return {6, 3};
    case eBuildingType::stadium: return {0, 0};

    case eBuildingType::bibliotheke: return {3, 3};
    case eBuildingType::university: return {-5, 3};
    case eBuildingType::observatory: return {7, 4};
    case eBuildingType::museum: return {12, 4};

    case eBuildingType::doricColumn:
    case eBuildingType::ionicColumn:
    case eBuildingType::corinthianColumn: return {4, 3};

    case eBuildingType::avenue: return {3, 3};

    case eBuildingType::commemorative: return {30, 6};
    case eBuildingType::godMonument: return {35, 7};

    case eBuildingType::park: return {3, 3};

    case eBuildingType::bench: return {2, 4};
    case eBuildingType::flowerGarden: return {8, 3};
    case eBuildingType::gazebo: return {6, 6};
    case eBuildingType::hedgeMaze: return {12, 4};
    case eBuildingType::fishPond: return {18, 6};

    case eBuildingType::waterPark: return {5, 4};

    case eBuildingType::birdBath: return {3, 3};
    case eBuildingType::shortObelisk: return {6, 2};
    case eBuildingType::tallObelisk: return {8, 2};
    case eBuildingType::shellGarden: return {5, 5};
    case eBuildingType::sundial: return {7, 4};
    case eBuildingType::dolphinSculpture: return {12, 4};
    case eBuildingType::orrery: return {9, 6};
    case eBuildingType::spring: return {10, 5};
    case eBuildingType::topiary: return {15, 3};
    case eBuildingType::baths: return {15, 6};
    case eBuildingType::stoneCircle: return {24, 4};

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
    case eBuildingType::templeZeus: return {20, 6};

    case eBuildingType::modestPyramid: return {15, 4};
    case eBuildingType::pyramid: return {20, 4};
    case eBuildingType::greatPyramid: return {25, 5};
    case eBuildingType::majesticPyramid: return {30, 6};

    case eBuildingType::smallMonumentToTheSky: return {16, 4};
    case eBuildingType::monumentToTheSky: return {20, 5};
    case eBuildingType::grandMonumentToTheSky: return {25, 6};

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
    case eBuildingType::minorShrineZeus: return {20, 4};

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
    case eBuildingType::shrineZeus: return {25, 5};

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
    case eBuildingType::majorShrineZeus: return {35, 6};

    case eBuildingType::pyramidOfThePantheon: return {30, 6};
    case eBuildingType::altarOfOlympus:  return {25, 5};
    case eBuildingType::templeOfOlympus:  return {30, 6};
    case eBuildingType::observatoryKosmika:  return {25, 6};
    case eBuildingType::museumAtlantika: return {25, 6};

    case eBuildingType::artisansGuild: return {-6, 3};

    case eBuildingType::achillesHall:
    case eBuildingType::atalantaHall:
    case eBuildingType::bellerophonHall:
    case eBuildingType::herculesHall:
    case eBuildingType::jasonHall:
    case eBuildingType::odysseusHall:
    case eBuildingType::perseusHall:
    case eBuildingType::theseusHall: return {15, 6};

    case eBuildingType::triremeWharf: return {-12, 4};

    case eBuildingType::chariotFactory: return {-10, 4};

    default: return {0, 0};
    }
}

eHeat eHeatGetters::housing(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::commonHouse: return {10, 5};
    case eBuildingType::eliteHousing: return {10, 5};
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
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::industry(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::timberMill: return {10, 5};
    case eBuildingType::masonryShop: return {10, 5};
    case eBuildingType::mint: return {10, 5};
    case eBuildingType::foundry: return {10, 5};
    case eBuildingType::sculptureStudio: return {10, 5};
    case eBuildingType::armory: return {10, 5};
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
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::farming(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::wheatFarm: return {10, 5};
    case eBuildingType::carrotsFarm: return {10, 5};
    case eBuildingType::onionsFarm: return {10, 5};
    case eBuildingType::growersLodge: return {10, 5};
    case eBuildingType::cardingShed: return {10, 5};
    case eBuildingType::dairy: return {10, 5};
    case eBuildingType::urchinQuay: return {10, 5};
    case eBuildingType::fishery: return {10, 5};
    case eBuildingType::corral: return {10, 5};
    case eBuildingType::huntingLodge: return {10, 5};
    case eBuildingType::orangeTendersLodge: return {10, 5};
    case eBuildingType::oliveTree:
    case eBuildingType::orangeTree:
    case eBuildingType::vine: return {2, 3};
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
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::storage(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::granary: return {10, 5};
    case eBuildingType::warehouse: return {10, 5};
    case eBuildingType::tradePost: return {10, 5};
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
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::culture(const eBuildingType type) {
    const bool a = eBuilding::sAestheticsBuilding(type);
    if(a) return {0, 0};
    const bool s = eBuilding::sSanctuaryBuilding(type);
    if(s) return {0, 0};
    switch(type) {
    case eBuildingType::college: return {10, 5};
    case eBuildingType::podium: return {10, 5};
    case eBuildingType::gymnasium: return {10, 5};
    case eBuildingType::dramaSchool: return {10, 5};
    case eBuildingType::theater: return {10, 5};
    case eBuildingType::stadium: return {10, 5};
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
        return {0, 0};
    default: return {2, 5};
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
        return {0, 0};
    default: return {-2, 5};
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
        return {0, 0};
    default: return {2, 5};
    }
}

eHeat eHeatGetters::fertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return {2, 8};
    } else {
        return {0, 0};
    }
}

eHeat eHeatGetters::notFertile(eTileBase* const tile) {
    const auto terr = tile->terrain();
    if(terr == eTerrain::fertile) {
        return {0, 0};
    } else {
        return {2, 8};
    }
}
