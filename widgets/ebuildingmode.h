#ifndef EBUILDINGMODE_H
#define EBUILDINGMODE_H

#include "buildings/ebuilding.h"

#include "characters/gods/egod.h"

enum class eBuildingMode {
    none,
    erase,

    road,
    roadblock,

    commonHousing,
    eliteHousing,

    wheatFarm,
    carrotFarm,
    onionFarm,

    vine,
    oliveTree,
    orangeTree,
    orangeTendersLodge,
    growersLodge,

    dairy,
    goat,
    cardingShed,
    sheep,
    cattle,

    fishery,
    urchinQuay,
    huntingLodge,
    corral,

    mint,
    foundry,
    timberMill,
    masonryShop,
    refinery,
    blackMarbleWorkshop,

    winery,
    olivePress,
    sculptureStudio,
    artisansGuild,
    armory,
    horseRanch,
    chariotFactory,

    triremeWharf,

    wall,
    tower,
    gatehouse,

    podium,
    college,

    theater,
    dramaSchool,

    gymnasium,
    stadium,

    bibliotheke,
    observatory,
    university,
    laboratory,
    inventorsWorkshop,
    museum,

    granary,
    warehouse,

    tradePost,
    pier,

    commonAgora,
    grandAgora,

    foodVendor,
    fleeceVendor,
    oilVendor,
    wineVendor,
    armsVendor,
    horseTrainer,
    chariotVendor,

    maintenanceOffice,
    fountain,
    hospital,
    watchpost,

    taxOffice,
    palace,
    bridge,

    park,
    doricColumn,
    ionicColumn,
    corinthianColumn,
    avenue,

    populationMonument,
    victoryMonument,
    colonyMonument,
    athleteMonument,
    conquestMonument,
    happinessMonument,
    heroicFigureMonument,
    diplomacyMonument,
    scholarMonument,

    aphroditeMonument,
    apolloMonument,
    aresMonument,
    artemisMonument,
    athenaMonument,
    atlasMonument,
    demeterMonument,
    dionysusMonument,
    hadesMonument,
    hephaestusMonument,
    heraMonument,
    hermesMonument,
    poseidonMonument,
    zeusMonument,

    bench,
    flowerGarden,
    gazebo,
    hedgeMaze,
    fishPond,

    waterPark,

    birdBath,
    shortObelisk,
    tallObelisk,
    shellGarden,
    sundial,
    dolphinSculpture,
    orrery,
    spring,
    topiary,
    baths,
    stoneCircle,

    templeAphrodite,
    templeApollo,
    templeAres,
    templeArtemis,
    templeAthena,
    templeAtlas,
    templeDemeter,
    templeDionysus,
    templeHades,
    templeHephaestus,
    templeHera,
    templeHermes,
    templePoseidon,
    templeZeus,

    achillesHall,
    atalantaHall,
    bellerophonHall,
    herculesHall,
    jasonHall,
    odysseusHall,
    perseusHall,
    theseusHall,

    modestPyramid, // 3x3
    pyramid, // 5x5
    greatPyramid, // 7x7
    majesticPyramid, // 9x9

    smallMonumentToTheSky, // 5x5
    monumentToTheSky, // 6x6
    grandMonumentToTheSky, // 8x8

    minorShrine, // 3x3
    shrine, // 6x6
    majorShrine, // 8x8

    pyramidToThePantheon, // 11x9
    altarOfOlympus, // 8x8
    templeOfOlympus, // 8x8
    observatoryKosmika, // 9x9
    museumAtlantika // 8x8
};

namespace eBuildingModeHelpers {
    eBuildingType toBuildingType(const eBuildingMode mode);
    int toCommemorativeId(const eBuildingMode mode);
    eGodType toGodType(const eBuildingMode mode);
}

#endif // EBUILDINGMODE_H
