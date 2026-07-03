#include "building-textures.h"

#include "binary-image-loader.h"
#include "offsets/SprAmbient.h"

#include "spriteData/palaceTiles30.h"

#include "spriteData/commonHouse30.h"

#include "spriteData/eliteHouse30.h"

#include "spriteData/college30.h"

#include "spriteData/gymnasium30.h"

#include "spriteData/dramaSchool30.h"

#include "spriteData/podium30.h"

#include "spriteData/theater30.h"

#include "spriteData/stadium30.h"

#include "spriteData/fountain30.h"

#include "spriteData/hospital30.h"

#include "spriteData/oliveTree30.h"

#include "spriteData/vine30.h"

#include "spriteData/plantation30.h"

#include "spriteData/huntingLodge30.h"

#include "spriteData/fishery30.h"

#include "spriteData/urchinQuay30.h"

#include "spriteData/cardingShed30.h"

#include "spriteData/dairy30.h"

#include "spriteData/growersLodge30.h"

#include "spriteData/timberMill30.h"

#include "spriteData/masonryShop30.h"

#include "spriteData/mint30.h"

#include "spriteData/foundry30.h"

#include "spriteData/artisansGuild30.h"

#include "spriteData/olivePress30.h"

#include "spriteData/winery30.h"

#include "spriteData/sculptureStudio30.h"

#include "spriteData/triremeWharf30.h"

#include "spriteData/triremeWharfOverlay130.h"

#include "spriteData/triremeWharfOverlay230.h"

#include "spriteData/horseRanch30.h"

#include "spriteData/horseRanchEnclosure30.h"

#include "spriteData/armory30.h"

#include "spriteData/gatehouseAndTower30.h"

#include "spriteData/wall30.h"

#include "spriteData/maintenanceOffice30.h"

#include "spriteData/taxOffice30.h"

#include "spriteData/watchpost30.h"

#include "spriteData/roadblock30.h"

#include "spriteData/bridge30.h"

#include "spriteData/mun_Palace30.h"

#include "spriteData/zeusSanctuaryElements30.h"

#include "spriteData/zeusHerosHall30.h"

#include "spriteData/poseidonHerosHall30.h"

#include "spriteData/poseidonStatues130.h"

#include "spriteData/poseidonStatues230.h"

#include "spriteData/stor_bays30.h"

#include "spriteData/tallObelisk30.h"

#include "spriteData/avenue30.h"

#include "spriteData/columns30.h"

#include "spriteData/commemorative30.h"

#include "spriteData/fishPond30.h"

#include "spriteData/hedgeMaze30.h"

#include "spriteData/gazebo30.h"

#include "spriteData/flowerGarden30.h"

#include "spriteData/bench30.h"

#include "spriteData/park30.h"

#include "spriteData/sundial30.h"

#include "spriteData/topiary30.h"

#include "spriteData/spring30.h"

#include "spriteData/stoneCircle30.h"

#include "spriteData/dolphinSculpture30.h"

#include "spriteData/orrery30.h"

#include "spriteData/shellGarden30.h"

#include "spriteData/storage30.h"

#include "spriteData/tradingPostOverlay30.h"

#include "spriteData/warehouseOverlay30.h"

#include "spriteData/fish30.h"

#include "spriteData/urchin30.h"

#include "spriteData/theaterOverlay30.h"

#include "spriteData/fisheryBoatBuilding30.h"

#include "spriteData/fisheryOverlay30.h"

#include "spriteData/urchinUnpackingOverlay30.h"

#include "spriteData/fisheryUnpackingOverlay30.h"

#include "spriteData/stadiumOverlays130.h"

#include "spriteData/sanctuaryOverlay30.h"

#include "spriteData/stadiumOverlays230.h"

#include "spriteData/sanctuaryFire30.h"

#include "spriteData/masonryShopOverlays30.h"

#include "spriteData/waitingOverlay30.h"

#include "spriteData/vendorOverlays30.h"

#include "spriteData/wineVendorOverlay30.h"

#include "spriteData/fishPondOverlay30.h"

#include "spriteData/watchPostOverlay30.h"

#include "spriteData/palaceOverlay30.h"

#include "spriteData/zeusStatue30.h"

#include "spriteData/poseidonStatue30.h"

#include "spriteData/hadesStatue30.h"

#include "spriteData/demeterStatue30.h"

#include "spriteData/athenaStatue30.h"

#include "spriteData/artemisStatue30.h"

#include "spriteData/apolloStatue30.h"

#include "spriteData/aresStatue30.h"

#include "spriteData/hephaestusStatue30.h"

#include "spriteData/aphroditeStatue30.h"

#include "spriteData/hermesStatue30.h"

#include "spriteData/dionysusStatue30.h"

#include "spriteData/blankStatue30.h"

#include "spriteData/atlasStatue30.h"

#include "spriteData/heraStatue30.h"

#include "spriteData/zeusSanctuary130.h"

#include "spriteData/zeusSanctuary230.h"

#include "spriteData/zeusSanctuary330.h"

#include "spriteData/orangeTendersLodge30.h"

#include "spriteData/waterPark30.h"

#include "spriteData/baths30.h"

#include "spriteData/birdBath30.h"

#include "spriteData/shortObelisk30.h"

#include "spriteData/agora30.h"

#include "spriteData/granary30.h"

#include "spriteData/pier130.h"

#include "spriteData/pier230.h"

#include "spriteData/warehouse30.h"

#include "spriteData/tradingPost30.h"

#include "spriteData/chariotVendor30.h"

#include "spriteData/chariotVendorOverlay30.h"

#include "spriteData/interfaceSprites30.h"

#include "spriteData/deerTop30.h"

#include "spriteData/altarSheepOverlay30.h"

#include "spriteData/altarGoodsOverlay30.h"

#include "spriteData/supplies30.h"

#include "spriteData/hippodrome30.h"

#include "offsets/PoseidonImps.h"

#include "textures/sprite-loader.h"

BuildingTextures::BuildingTextures(const int tileW, const int tileH,
                                     SDL_Renderer* const renderer) :
    fTileW(tileW), fTileH(tileH),
    fRenderer(renderer),

    fEliteHouseHorses(renderer),

    fGymnasiumOverlay(renderer),
    fCollegeOverlay(renderer),
    fDramaSchoolOverlay(renderer),
    fPodiumOverlay(renderer),
    fTheaterOverlay(renderer),
    fStadiumOverlay1(renderer),
    fStadiumOverlay2(renderer),
    fStadiumOverlay3(renderer),
    fStadiumOverlay4W(renderer),
    fStadiumOverlay4H(renderer),
    fStadiumOverlay5W(renderer),
    fStadiumOverlay5H(renderer),
    fStadiumAudiance1W(renderer),
    fStadiumAudiance2W(renderer),
    fStadiumAudiance1H(renderer),
    fStadiumAudiance2H(renderer),

    fBibliothekeOverlay(renderer),
    fObservatoryOverlay(renderer),
    fUniversityOverlay(renderer),
    fLaboratoryOverlay(renderer),
    fInventorsWorkshopOverlay(renderer),
    fMuseumOverlay(renderer),

    fPalaceHOverlay(renderer),
    fPalaceWOverlay(renderer),

    fPalaceTiles(renderer),

    fFountainOverlay(renderer),
    fHospitalOverlay(renderer),

    fOliveTree(renderer),
    fVine(renderer),
    fOrangeTree(renderer),

    fWheat(renderer),
    fCarrots(renderer),
    fOnions(renderer),

    fHuntingLodgeOverlay(renderer),
    fFishery(renderer),
    fFisheryBoatBuildingW(renderer),
    fFisheryBoatBuildingH(renderer),
    fFisheryUnpackingOverlayTL(renderer),
    fFisheryUnpackingOverlayTR(renderer),
    fFisheryUnpackingOverlayBL(renderer),
    fFisheryUnpackingOverlayBR(renderer),
    fUrchinQuay(renderer),
    fUrchinQuayUnpackingOverlayTL(renderer),
    fUrchinQuayUnpackingOverlayTR(renderer),
    fUrchinQuayUnpackingOverlayBL(renderer),
    fUrchinQuayUnpackingOverlayBR(renderer),
    fCardingShedOverlay(renderer),
    fDairyOverlay(renderer),
    fGrowersLodgeOverlay(renderer),
//    fCorralOverlay(renderer),
    fOrangeTendersLodgeOverlay(renderer),

    fTimberMillOverlay(renderer),
    fMasonryShopStones(renderer),
    fBlackMarbleWorkshopStones(renderer),
    fRefineryOverlay(renderer),
    fMintOverlay(renderer),
    fFoundryOverlay(renderer),
    fArtisansGuildOverlay(renderer),
    fOlivePressOverlay(renderer),
    fWineryOverlay(renderer),
    fSculptureStudioOverlay(renderer),

    fTriremeWharf(renderer),
    fTriremeWharfOverlay1BL(renderer),
    fTriremeWharfOverlay1BR(renderer),
    fTriremeWharfOverlay1TL(renderer),
    fTriremeWharfOverlay1TR(renderer),
    fTriremeWharfOverlay2BL(renderer),
    fTriremeWharfOverlay2BR(renderer),
    fTriremeWharfOverlay2TL(renderer),
    fTriremeWharfOverlay2TR(renderer),

    fHorseRanchOverlay(renderer),
    fCorralOverlay(renderer),
    fCorralProcessingOverlay(renderer),
    fArmoryOverlay(renderer),

    fGatehouseW(renderer),
    fGatehouseH(renderer),
    fWall(renderer),

    fMaintenanceOfficeOverlay(renderer),
    fTaxOfficeOverlay(renderer),
    fWatchPostOverlay(renderer),

    fWaitingMeat(renderer),
    fWaitingCheese(renderer),
    fWaitingWheat(renderer),
    fWaitingOranges(renderer),
    fWaitingWood(renderer),
    fWaitingBronze(renderer),
    fWaitingGrapes(renderer),
    fWaitingOlives(renderer),
    fWaitingArmor(renderer),
    fWaitingOrichalc(renderer),

    fAgoraRoad(renderer),
    fAgora(renderer),

    fFoodVendorOverlay2(renderer),
    fFleeceVendorOverlay2(renderer),
    fOilVendorOverlay2(renderer),
    fArmsVendorOverlay2(renderer),
    fWineVendorOverlay2(renderer),
    fHorseTrainerOverlay2(renderer),

    fChariotVendorOverlay2(renderer),

    fChariotFactoryOverlay(renderer),
    fChariotFactoryOverlay1(renderer),
    fChariotFactoryChariots(renderer),

    fWarehouseOverlay(renderer),

    fPier1(renderer),
    fPierOverlay(renderer),
    fPierLoadOverlay(renderer),
    fTradingPostOverlay(renderer),

    fWarehouseUrchin(renderer),
    fWarehouseFish(renderer),
    fWarehouseMeat(renderer),
    fWarehouseCheese(renderer),
    fWarehouseCarrots(renderer),
    fWarehouseOnions(renderer),
    fWarehouseWheat(renderer),
    fWarehouseOranges(renderer),
    fWarehouseBlackMarble(renderer),
    fWarehouseOrichalc(renderer),
    fWarehouseWood(renderer),
    fWarehouseBronze(renderer),
    fWarehouseMarble(renderer),
    fWarehouseGrapes(renderer),
    fWarehouseOlives(renderer),
    fWarehouseFleece(renderer),
    fWarehouseArmor(renderer),
    fWarehouseOliveOil(renderer),
    fWarehouseWine(renderer),

    fGranaryOverlay(renderer),

    fPark(renderer),
    fLargePark(renderer),
    fHugePark(renderer),

    fWaterPark1Overlay(renderer),
    fWaterPark2Overlay(renderer),
    fWaterPark3Overlay(renderer),
    fWaterPark4Overlay(renderer),
    fWaterPark5Overlay(renderer),
    fWaterPark6Overlay(renderer),
    fWaterPark7Overlay(renderer),
    fWaterPark8Overlay(renderer),

    fFishPondOverlay(renderer),

    fAvenueRoad(renderer),

    fCommemorative(renderer),

    fBirdBathOverlay(renderer),
    fSpring(renderer),
    fBathsOverlay(renderer),
    fStoneCircleOverlay(renderer),

    fZeusStatues(renderer),
    fPoseidonStatues(renderer),
    fHadesStatues(renderer),
    fDemeterStatues(renderer),
    fAthenaStatues(renderer),
    fArtemisStatues(renderer),
    fApolloStatues(renderer),
    fAresStatues(renderer),
    fHephaestusStatues(renderer),
    fAphroditeStatues(renderer),
    fHermesStatues(renderer),
    fDionysusStatues(renderer),

    fHeraStatues(renderer),
    fAtlasStatues(renderer),

    fSanctuaryTiles(renderer),
    fSanctuarySpace(renderer),
    fAltarBullOverlay(renderer),
    fAltarSheepOverlay(renderer),
    fAltarGoodsOverlay(renderer),
    fAltarBullOverlayFlipped(renderer),
    fAltarSheepOverlayFlipped(renderer),
    fAltarGoodsOverlayFlipped(renderer),

    fHeroStatues(renderer),

    fZeusMonuments(renderer),
    fPoseidonMonuments(renderer),
    fHadesMonuments(renderer),
    fDemeterMonuments(renderer),
    fAthenaMonuments(renderer),
    fArtemisMonuments(renderer),
    fApolloMonuments(renderer),
    fAresMonuments(renderer),
    fHephaestusMonuments(renderer),
    fAphroditeMonuments(renderer),
    fHermesMonuments(renderer),
    fDionysusMonuments(renderer),

    fHeraMonuments(renderer),
    fAtlasMonuments(renderer),

    fSanctuaryFlippedSW(renderer),
    fSanctuaryFlippedNW(renderer),
    fPoseidonSanctuary(renderer),
    fPyramid(renderer),
    fPyramid2(renderer),

    fSanctuaryWOverlay(renderer),
    fSanctuaryHOverlay(renderer),
    fSanctuaryFire(renderer),

    fFish(renderer),
    fUrchin(renderer),

    fBridge(renderer),
    fPoseidonBridge(renderer),

    fColumn1(renderer),
    fColumn2(renderer),
    fColumn3(renderer),
    fColumn4(renderer),
    fColumn5(renderer),

    fSpawner(renderer),

    fHippodrome(renderer) {}

void BuildingTextures::loadAll() {
    load();

    loadCommonHouse();
    loadPoseidonCommonHouse();
    loadEliteHouse();
    loadPoseidonEliteHouse();
    loadCollege();
    loadGymnasium();
    loadDramaSchool();
    loadPodium();
    loadTheater();
    loadStadium();
    loadFountain();
    loadHospital();
    loadOliveTree();
    loadVine();
    loadPlantation();
    loadHuntingLodge();
    loadFishery();
    loadUrchinQuay();
    loadCardingShed();
    loadDairy();
    loadGrowersLodge();
    loadTimberMill();
    loadMasonryShop();
    loadBlackMarbleWorkshop();
    loadRefinery();
    loadOrichalcTowerOverlay();
    loadMasonryShopOverlays();
    loadMint();
    loadFoundry();
    loadArtisansGuild();
    loadOlivePress();
    loadWinery();
    loadSculptureStudio();
    loadTriremeWharf();
    loadHorseRanch();
    loadArmory();
    loadGatehouseAndTower();
    loadWall();
    loadMaintenanceOffice();
    loadTaxOffice();
    loadWatchpost();
    loadRoadblock();
    loadBridge();
    loadPoseidonBridge();
    loadPalace();
    loadPalaceTiles();
    loadSanctuary();
    loadZeusSanctuary();
    loadPoseidonSanctuary();
    loadPyramid();
    loadPyramid2();
    loadPoseidonHerosHall();
    loadHerosHall();
    loadWaitingOverlay();
    loadOrangeTendersLodge();
    loadWaterPark();
    loadShortObelisk();
    loadBirdBath();
    loadBaths();
    loadShellGarden();
    loadOrrery();
    loadDolphinSculpture();
    loadStoneCircle();
    loadSpring();
    loadTopiary();
    loadSundial();
    loadTallObelisk();
    loadAvenue();
    loadColumns();
    loadCommemorative();
    loadFishPond();
    loadHedgeMaze();
    loadGazebo();
    loadFlowerGarden();
    loadBench();
    loadPark();

    loadZeusMonuments();
    loadPoseidonMonuments();
    loadHadesMonuments();
    loadDemeterMonuments();
    loadAthenaMonuments();
    loadArtemisMonuments();
    loadApolloMonuments();
    loadAresMonuments();
    loadHephaestusMonuments();
    loadAphroditeMonuments();
    loadHermesMonuments();
    loadDionysusMonuments();

    loadHeraMonuments();
    loadAtlasMonuments();

    loadAgora();
    loadGranary();
    loadPier();
    loadTradingPost();

    loadBibliotheke();
    loadObservatory();
    loadUniversity();
    loadLaboratory();
    loadInventorsWorkshop();
    loadMuseum();

    loadCorral();

    loadChariotFactory();

    loadChariotVendor();

    loadWineVendor();

    loadAltarBullOverlay();
    loadAltarSheepOverlay();
    loadAltarGoodsOverlay();
}

void BuildingTextures::loadAltarSheepOverlay() {
    if(fAltarSheepOverlayLoaded) return;
    fAltarSheepOverlayLoaded = true;

    const auto& sds = eAltarSheepOverlaySpriteData30;
    SpriteLoader loader(fTileH, "altarSheepOverlay", sds,
                         nullptr, fRenderer);

    for(int i = 493; i < 517; i++) {
        loader.load(493, i, fAltarSheepOverlay);
    }
    generateFlipped(fAltarSheepOverlay, fAltarSheepOverlayFlipped);
}

void BuildingTextures::loadAltarGoodsOverlay() {
    if(fAltarGoodsOverlayLoaded) return;
    fAltarGoodsOverlayLoaded = true;

    const auto& sds = eAltarGoodsOverlaySpriteData30;
    SpriteLoader loader(fTileH, "altarGoodsOverlay", sds,
                         nullptr, fRenderer);

    for(int i = 517; i < 529; i++) {
        loader.load(517, i, fAltarGoodsOverlay);
    }
    generateFlipped(fAltarGoodsOverlay, fAltarGoodsOverlayFlipped);
}

void BuildingTextures::loadCommonHouse() {
    if(fCommonHouseLoaded) return;
    fCommonHouseLoaded = true;

    const auto& sds = eCommonHouseSpriteData30;
    SpriteLoader loader(fTileH, "commonHouse", sds,
                         nullptr, fRenderer);

    fHouseSpace = loader.load(1, 15);
    for(int i = 1; i < 15;) {
        auto& coll = fCommonHouse.emplace_back(fRenderer);
        for(int j = 0; j < 2; j++, i++) {
            loader.load(1, i, coll);
        }
    }
}

void BuildingTextures::loadEliteHouse() {
    if(fEliteHouseLoaded) return;
    fEliteHouseLoaded = true;
    const auto& sds = eEliteHouseSpriteData30;
    SpriteLoader loader(fTileH, "eliteHouse", sds,
                         nullptr, fRenderer);

    for(int i = 16; i < 46;) {
        auto& coll = fEliteHouse.emplace_back(fRenderer);
        for(int j = 0; j < 5; j++, i++) {
            loader.load(16, i, coll);
        }
    }

    for(int i = 46; i < 50; i++) {
        loader.load(16, i, fEliteHouseHorses);
    }
}

void BuildingTextures::loadCollege() {
    if(fCollegeLoaded) return;
    fCollegeLoaded = true;

    const auto& sds = eCollegeSpriteData30;
    SpriteLoader loader(fTileH, "college", sds,
                         nullptr, fRenderer);
    fCollege = loader.load(1, 1);

    for(int i = 2; i < 26; i++) {
        loader.load(1, i, fCollegeOverlay);
    }
}

void BuildingTextures::loadGymnasium() {
    if(fGymnasiumLoaded) return;
    fGymnasiumLoaded = true;

    const auto& sds = eGymnasiumSpriteData30;
    SpriteLoader loader(fTileH, "gymnasium", sds,
                         nullptr, fRenderer);

    fGymnasium = loader.load(26, 26);

    for(int i = 27; i < 59; i++) {
        loader.load(26, i, fGymnasiumOverlay);
    }
}

void BuildingTextures::loadDramaSchool() {
    if(fDramaSchoolLoaded) return;
    fDramaSchoolLoaded = true;

    const auto& sds = eDramaSchoolSpriteData30;
    SpriteLoader loader(fTileH, "dramaSchool", sds,
                         nullptr, fRenderer);
    fDramaSchool = loader.load(59, 59);

    for(int i = 60; i < 84; i++) {
        loader.load(59, i, fDramaSchoolOverlay);
    }
}

void BuildingTextures::loadPodium() {
    if(fPodiumLoaded) return;
    fPodiumLoaded = true;

    const auto& sds = ePodiumSpriteData30;
    SpriteLoader loader(fTileH, "podium", sds,
                         nullptr, fRenderer);
    fPodium = loader.load(84, 84);

    for(int i = 85; i < 109; i++) {
        loader.load(84, i, fPodiumOverlay);
    }
}

void BuildingTextures::loadTheater() {
    if(fTheaterLoaded) return;
    fTheaterLoaded = true;

    {
        const auto& sds = eTheaterSpriteData30;
        SpriteLoader loader(fTileH, "theater", sds,
                             nullptr, fRenderer);
        fTheater = loader.load(109, 109);
    }


    {
        const auto& sds = eTheaterOverlaySpriteData30;
        SpriteLoader loader(fTileH, "theaterOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 656; i < 680; i++) {
            loader.load(656, i, fTheaterOverlay);
        }
    }
}

void BuildingTextures::loadStadium() {
    if(fStadiumLoaded) return;
    fStadiumLoaded = true;

    {
        const auto& sds = eStadiumSpriteData30;
        SpriteLoader loader(fTileH, "stadium", sds,
                             nullptr, fRenderer);
        fStadium2H = loader.load(110, 110);
        fStadium1H = loader.load(110, 111);
        fStadium1W = loader.load(110, 112);
        fStadium2W = loader.load(110, 113);
    }

    {
        const auto& sds = eStadiumOverlays1SpriteData30;
        SpriteLoader loader(fTileH, "stadiumOverlays1", sds,
                             &eSprAmbientOffset, fRenderer);


        for(int i = 1492; i < 1510; i++) {
            loader.load(1492, i, fStadiumOverlay1);
        }

        for(int i = 1510; i < 1530; i++) {
            loader.load(1492, i, fStadiumOverlay2);
        }

        for(int i = 1530; i < 1547; i++) {
            loader.load(1492, i, fStadiumOverlay3);
        }

        for(int i = 1547; i < 1629;) {
            for(int j = 0; j < 2; j++, i++) {
                TextureCollection* coll;
                if(j == 0) {
                    coll = &fStadiumOverlay4W;
                } else {
                    coll = &fStadiumOverlay4H;
                }
                loader.load(1492, i, *coll);
            }
        }

        for(int i = 1629; i < 1653; i++) {
            loader.load(1492, i, fStadiumAudiance1W);
        }

        for(int i = 1653; i < 1677; i++) {
            loader.load(1492, i, fStadiumAudiance2W);
        }

        for(int i = 1677; i < 1701; i++) {
            loader.load(1492, i, fStadiumAudiance1H);
        }

        for(int i = 1701; i < 1725; i++) {
            loader.load(1492, i, fStadiumAudiance2H);
        }
    }
    {

        const auto& sds = eStadiumOverlays2SpriteData30;
        SpriteLoader loader(fTileH, "stadiumOverlays2", sds,
                             &eSprAmbientOffset, fRenderer);


        for(int i = 1773; i < 1937;) {
            for(int j = 0; j < 2; j++, i++) {
                TextureCollection* coll;
                if(j == 0) {
                    coll = &fStadiumOverlay5W;
                } else {
                    coll = &fStadiumOverlay5H;
                }
                loader.load(1773, i, *coll);
            }
        }
    }
}

void BuildingTextures::loadFountain() {
    if(fFountainLoaded) return;
    fFountainLoaded = true;

    const auto& sds = eFountainSpriteData30;
    SpriteLoader loader(fTileH, "fountain", sds,
                         nullptr, fRenderer);

    fFountain = loader.load(1, 1);
    for(int i = 2; i < 8; i++) {
        loader.load(1, i, fFountainOverlay);
    }
}

void BuildingTextures::loadHospital() {
    if(fHospitalLoaded) return;
    fHospitalLoaded = true;

    const auto& sds = eHospitalSpriteData30;
    SpriteLoader loader(fTileH, "hospital", sds,
                         nullptr, fRenderer);
    fHospital = loader.load(8, 8);
    for(int i = 9; i < 21; i++) {
        loader.load(8, i, fHospitalOverlay);
    }
}

void BuildingTextures::loadOliveTree() {
    if(fOliveTreeLoaded) return;
    fOliveTreeLoaded = true;

    const auto& sds = eOliveTreeSpriteData30;
    SpriteLoader loader(fTileH, "oliveTree", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 7; i++) {
        loader.load(1, i, fOliveTree);
    }
}

void BuildingTextures::loadVine() {
    if(fVineLoaded) return;
    fVineLoaded = true;

    const auto& sds = eVineSpriteData30;
    SpriteLoader loader(fTileH, "vine", sds,
                         nullptr, fRenderer);

    for(int i = 7; i < 13; i++) {
        loader.load(7, i, fVine);
    }
}

void BuildingTextures::loadPlantation() {
    if(fPlantationLoaded) return;
    fPlantationLoaded = true;

    const auto& sds = ePlantationSpriteData30;
    SpriteLoader loader(fTileH, "plantation", sds,
                         nullptr, fRenderer);

    fPlantation = loader.load(13, 13);

    for(int i = 14; i < 20; i++) {
        loader.load(13, i, fWheat);
    }

    for(int i = 20; i < 26; i++) {
        loader.load(13, i, fCarrots);
    }

    for(int i = 26; i < 32; i++) {
        loader.load(13, i, fOnions);
    }
}

void BuildingTextures::loadHuntingLodge() {
    if(fHuntingLodgeLoaded) return;
    fHuntingLodgeLoaded = true;

    const auto& sds = eHuntingLodgeSpriteData30;
    SpriteLoader loader(fTileH, "huntingLodge", sds,
                         nullptr, fRenderer);

    fHuntingLodge = loader.load(32, 32);
    for(int i = 33; i < 48; i++) {
        loader.load(32, i, fHuntingLodgeOverlay);
    }
}

void BuildingTextures::loadFishery() {
    if(fFisheryLoaded) return;
    fFisheryLoaded = true;

    {
        const auto& sds = eFisherySpriteData30;
        SpriteLoader loader(fTileH, "fishery", sds,
                             nullptr, fRenderer);

        for(int i = 48; i < 52; i++) {
            loader.load(48, i, fFishery);
        }
    }
    {
        const auto& sds = eFisheryBoatBuildingSpriteData30;
        SpriteLoader loader(fTileH, "fisheryBoatBuilding", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 982; i < 992; i++) {
            loader.load(982, i, fFisheryBoatBuildingW);
        }
        for(int i = 992; i < 1002; i++) {
            loader.load(982, i, fFisheryBoatBuildingH);
        }
    }
    {
        const auto& sds = eFisheryUnpackingOverlaySpriteData30;
        SpriteLoader loader(fTileH, "fisheryUnpackingOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 890; i < 910; i++) {
            loader.load(890, i, fFisheryUnpackingOverlayTL);
        }

        for(int i = 910; i < 930; i++) {
            loader.load(890, i, fFisheryUnpackingOverlayTR);
        }

        for(int i = 930; i < 950; i++) {
            loader.load(890, i, fFisheryUnpackingOverlayBL);
        }

        for(int i = 950; i < 970; i++) {
            loader.load(890, i, fFisheryUnpackingOverlayBR);
        }
    }

    loadFisheryOverlay();
}

void BuildingTextures::loadFisheryOverlay() {
    if(fFisheryOverlayLoaded) return;
    fFisheryOverlayLoaded = true;

    const auto& sds = eFisheryOverlaySpriteData30;
    SpriteLoader loader(fTileH, "fisheryOverlay", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int j = 0; j < 8; j++) {
        fFisheryOverlay.emplace_back(fRenderer);
    }

    for(int i = 2117; i < 2213;) {
        for(int j = 0; j < 8; j++, i++) {
            if(j > 3 && j < 7) continue;
            loader.load(2117, i, fFisheryOverlay[j]);
        }
    }
    for(int i = 2117; i < 2213;) {
        for(int j = 0; j < 8; j++, i++) {
            if(j > 3 && j < 7) continue;
            loader.load(2117, i, fFisheryOverlay[j]);
        }
    }

    generateFlipped(fFisheryOverlay[0], fFisheryOverlay[6]);
    generateFlipped(fFisheryOverlay[1], fFisheryOverlay[5]);
    generateFlipped(fFisheryOverlay[2], fFisheryOverlay[4]);
}

void BuildingTextures::loadUrchinQuay() {
    if(fUrchinQuayLoaded) return;
    fUrchinQuayLoaded = true;

    {
        const auto& sds = eUrchinQuaySpriteData30;
        SpriteLoader loader(fTileH, "urchinQuay", sds,
                             nullptr, fRenderer);

        for(int i = 52; i < 56; i++) {
            loader.load(52, i, fUrchinQuay);
        }
    }
    {
        const auto& sds = eUrchinUnpackingOverlaySpriteData30;
        SpriteLoader loader(fTileH, "urchinUnpackingOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 680; i < 730; i++) {
            loader.load(680, i, fUrchinQuayUnpackingOverlayTL);
        }

        for(int i = 730; i < 780; i++) {
            loader.load(680, i, fUrchinQuayUnpackingOverlayTR);
        }

        for(int i = 780; i < 830; i++) {
            loader.load(680, i, fUrchinQuayUnpackingOverlayBL);
        }

        for(int i = 830; i < 880; i++) {
            loader.load(680, i, fUrchinQuayUnpackingOverlayBR);
        }
    }

    loadFisheryOverlay();
}

void BuildingTextures::loadCardingShed() {
    if(fCardingShedLoaded) return;
    fCardingShedLoaded = true;

    const auto& sds = eCardingShedSpriteData30;
    SpriteLoader loader(fTileH, "cardingShed", sds,
                         nullptr, fRenderer);

    fCardingShed = loader.load(56, 56);
    for(int i = 57; i < 69; i++) {
        loader.load(56, i, fCardingShedOverlay);
    }
}

void BuildingTextures::loadDairy() {
    if(fDairyLoaded) return;
    fDairyLoaded = true;

    const auto& sds = eDairySpriteData30;
    SpriteLoader loader(fTileH, "dairy", sds,
                         nullptr, fRenderer);

    fDairy = loader.load(69, 69);
    for(int i = 70; i < 91; i++) {
        loader.load(69, i, fDairyOverlay);
    }
}

void BuildingTextures::loadGrowersLodge() {
    if(fGrowersLodgeLoaded) return;
    fGrowersLodgeLoaded = true;

    const auto& sds = eGrowersLodgeSpriteData30;
    SpriteLoader loader(fTileH, "growersLodge", sds,
                         nullptr, fRenderer);

    fGrowersLodge = loader.load(91, 91);
    for(int i = 92; i < 102; i++) {
        loader.load(91, i, fGrowersLodgeOverlay);
    }
}

void BuildingTextures::loadTimberMill() {
    if(fTimberMillLoaded) return;
    fTimberMillLoaded = true;

    const auto& sds = eTimberMillSpriteData30;
    SpriteLoader loader(fTileH, "timberMill", sds,
                         nullptr, fRenderer);

    fTimberMill = loader.load(1, 1);
    for(int i = 2; i < 12; i++) {
        loader.load(1, i, fTimberMillOverlay);
    }
}

void BuildingTextures::loadMasonryShop() {
    if(fMasonryShopLoaded) return;
    fMasonryShopLoaded = true;

    const auto& sds = eMasonryShopSpriteData30;
    SpriteLoader loader(fTileH, "masonryShop", sds,
                         nullptr, fRenderer);

    fMasonryShop = loader.load(12, 12);
    for(int i = 13; i < 21; i++) {
        loader.load(12, i, fMasonryShopStones);
    }

    loadMasonryShopOverlays();
}

void BuildingTextures::loadMasonryShopOverlays() {
    if(fMasonryShopOverlaysLoaded) return;
    fMasonryShopOverlaysLoaded = true;

    const auto& sds = eMasonryShopOverlaysSpriteData30;
    SpriteLoader loader(fTileH, "masonryShopOverlays", sds,
                         &eSprAmbientOffset, fRenderer);

    loader.loadSkipFlipped(1937, 1937, 2017, fMasonryShopOverlay1);
    loader.loadSkipFlipped(1937, 2017, 2097, fMasonryShopOverlay2);
}

void BuildingTextures::loadMint() {
    if(fMintLoaded) return;
    fMintLoaded = true;

    const auto& sds = eMintSpriteData30;
    SpriteLoader loader(fTileH, "mint", sds,
                         nullptr, fRenderer);

    fMint = loader.load(21, 21);
    for(int i = 22; i < 32; i++) {
        loader.load(21, i, fMintOverlay);
    }
}

void BuildingTextures::loadFoundry() {
    if(fFoundryLoaded) return;
    fFoundryLoaded = true;

    const auto& sds = eFoundrySpriteData30;
    SpriteLoader loader(fTileH, "foundry", sds,
                         nullptr, fRenderer);

    fFoundry = loader.load(36, 36);
    for(int i = 37; i < 49; i++) {
        loader.load(36, i, fFoundryOverlay);
    }
}

void BuildingTextures::loadArtisansGuild() {
    if(fArtisansGuildLoaded) return;
    fArtisansGuildLoaded = true;

    const auto& sds = eArtisansGuildSpriteData30;
    SpriteLoader loader(fTileH, "artisansGuild", sds,
                         nullptr, fRenderer);

    fArtisansGuild = loader.load(49, 49);
    for(int i = 50; i < 60; i++) {
        loader.load(49, i, fArtisansGuildOverlay);
    }
}

void BuildingTextures::loadOlivePress() {
    if(fOlivePressLoaded) return;
    fOlivePressLoaded = true;

    const auto& sds = eOlivePressSpriteData30;
    SpriteLoader loader(fTileH, "olivePress", sds,
                         nullptr, fRenderer);

    fOlivePress = loader.load(60, 60);
    for(int i = 61; i < 73; i++) {
        loader.load(60, i, fOlivePressOverlay);
    }
}

void BuildingTextures::loadWinery() {
    if(fWineryLoaded) return;
    fWineryLoaded = true;

    const auto& sds = eWinerySpriteData30;
    SpriteLoader loader(fTileH, "winery", sds,
                         nullptr, fRenderer);

    fWinery = loader.load(73, 73);
    for(int i = 74; i < 86; i++) {
        loader.load(73, i, fWineryOverlay);
    }
}

void BuildingTextures::loadSculptureStudio() {
    if(fSculptureStudioLoaded) return;
    fSculptureStudioLoaded = true;
    const auto& sds = eSculptureStudioSpriteData30;
    SpriteLoader loader(fTileH, "sculptureStudio", sds,
                         nullptr, fRenderer);

    fSculptureStudio = loader.load(86, 86);
    for(int i = 87; i < 122; i++) {
        loader.load(86, i, fSculptureStudioOverlay);
    }
}

void BuildingTextures::loadTriremeWharf() {
    if(fTriremeWharfLoaded) return;
    fTriremeWharfLoaded = true;

    {
        const auto& sds = eTriremeWharfSpriteData30;
        SpriteLoader loader(fTileH, "triremeWharf", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 5; i++) {
            loader.load(1, i, fTriremeWharf);
        }
    }
    {
        const auto& sds = eTriremeWharfOverlay1SpriteData30;
        SpriteLoader loader(fTileH, "triremeWharfOverlay1", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 1338; i < 1366; i++) {
            loader.load(1338, i, fTriremeWharfOverlay1BL);
        }
        for(int i = 1366; i < 1394; i++) {
            loader.load(1338, i, fTriremeWharfOverlay1BR);
        }
        for(int i = 1394; i < 1422; i++) {
            loader.load(1338, i, fTriremeWharfOverlay1TL);
        }
        for(int i = 1422; i < 1450; i++) {
            loader.load(1338, i, fTriremeWharfOverlay1TR);
        }
    }
    {
        const auto& sds = eTriremeWharfOverlay2SpriteData30;
        SpriteLoader loader(fTileH, "triremeWharfOverlay2", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 1450; i < 1460; i++) {
            loader.load(1450, i, fTriremeWharfOverlay2BL);
        }
        for(int i = 1460; i < 1470; i++) {
            loader.load(1450, i, fTriremeWharfOverlay2BR);
        }
        for(int i = 1470; i < 1480; i++) {
            loader.load(1450, i, fTriremeWharfOverlay2TL);
        }
        for(int i = 1480; i < 1490; i++) {
            loader.load(1450, i, fTriremeWharfOverlay2TR);
        }
    }
}

void BuildingTextures::loadHorseRanch() {
    if(fHorseRanchLoaded) return;
    fHorseRanchLoaded = true;

    {
        const auto& sds = eHorseRanchSpriteData30;
        SpriteLoader loader(fTileH, "horseRanch", sds,
                             nullptr, fRenderer);

        fHorseRanch = loader.load(5, 5);
        for(int i = 6; i < 22; i++) {
            loader.load(5, i, fHorseRanchOverlay);
        }
    }
    {
        const auto& sds = HorseRanchEnclosureSpriteData30;
        SpriteLoader loader(fTileH, "horseRanchEnclosure", sds,
                             nullptr, fRenderer);

        fHorseRanchEnclosure = loader.load(41, 41);
    }
}

void BuildingTextures::loadArmory() {
    if(fArmoryLoaded) return;
    fArmoryLoaded = true;

    const auto& sds = eArmorySpriteData30;
    SpriteLoader loader(fTileH, "armory", sds,
                         nullptr, fRenderer);

    fArmory = loader.load(22, 22);
    for(int i = 23; i < 41; i++) {
        loader.load(22, i, fArmoryOverlay);
    }
}

void BuildingTextures::loadGatehouseAndTower() {
    if(fGatehouseAndTowerLoaded) return;
    fGatehouseAndTowerLoaded = true;

    const auto& sds = eGatehouseAndTowerSpriteData30;
    SpriteLoader loader(fTileH, "gatehouseAndTower", sds,
                         nullptr, fRenderer);

    loader.load(50, 50, fGatehouseW);
    loader.load(50, 52, fGatehouseW);
    loader.load(50, 54, fGatehouseW);
    loader.load(50, 57, fGatehouseW);

    loader.load(50, 51, fGatehouseH);
    loader.load(50, 53, fGatehouseH);
    loader.load(50, 55, fGatehouseH);
    loader.load(50, 58, fGatehouseH);

    fTower = loader.load(50, 56);
}

void BuildingTextures::loadWall() {
    if(fWallLoaded) return;
    fWallLoaded = true;

    const auto& sds = eWallSpriteData30;
    SpriteLoader loader(fTileH, "wall", sds,
                         nullptr, fRenderer);

    for(int i = 59; i < 95; i++) {
        loader.load(59, i, fWall);
    }
}

void BuildingTextures::loadMaintenanceOffice() {
    if(fMaintenanceOfficeLoaded) return;
    fMaintenanceOfficeLoaded = true;

    const auto& sds = eMaintenanceOfficeSpriteData30;
    SpriteLoader loader(fTileH, "maintenanceOffice", sds,
                         nullptr, fRenderer);

    fMaintenanceOffice = loader.load(1, 1);
    for(int i = 2; i < 46; i++) {
        loader.load(1, i, fMaintenanceOfficeOverlay);
    }
}

void BuildingTextures::loadTaxOffice() {
    if(fTaxOfficeLoaded) return;
    fTaxOfficeLoaded = true;

    const auto& sds = eTaxOfficeSpriteData30;
    SpriteLoader loader(fTileH, "taxOffice", sds,
                         nullptr, fRenderer);

    fTaxOffice = loader.load(46, 46);
    for(int i = 47; i < 65; i++) {
        loader.load(46, i, fTaxOfficeOverlay);
    }
}

void BuildingTextures::loadWatchpost() {
    if(fWatchpostLoaded) return;
    fWatchpostLoaded = true;

    {
        const auto& sds = eWatchpostSpriteData30;
        SpriteLoader loader(fTileH, "watchpost", sds,
                             nullptr, fRenderer);

        fWatchPost = loader.load(65, 65);
    }

    {
        const auto& sds = eWatchPostOverlaySpriteData30;
        SpriteLoader loader(fTileH, "watchPostOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 2958; i < 3011; i++) {
            loader.load(2958, i, fWatchPostOverlay);
        }
    }
}

void BuildingTextures::loadRoadblock() {
    if(fRoadblockLoaded) return;
    fRoadblockLoaded = true;

    const auto& sds = eRoadblockSpriteData30;
    SpriteLoader loader(fTileH, "roadblock", sds,
                         nullptr, fRenderer);

    fRoadblock = loader.load(76, 76);
}

void BuildingTextures::loadBridge() {
    if(fBridgeLoaded) return;
    fBridgeLoaded = true;

    const auto& sds = eBridgeSpriteData30;
    SpriteLoader loader(fTileH, "bridge", sds,
                         nullptr, fRenderer);

    for(int i = 77; i < 89; i++) {
        loader.load(77, i, fBridge);
    }
}

void BuildingTextures::loadPalaceTiles() {
    if(fPalaceTilesLoaded) return;
    fPalaceTilesLoaded = true;

    const auto& sds = ePalaceTilesSpriteData30;
    SpriteLoader loader(fTileH, "palaceTiles", sds,
                         nullptr, fRenderer);

    loader.load(25, 25, fPalaceTiles);
    loader.load(26, 26, fPalaceTiles);
}

void BuildingTextures::loadPalace() {
    if(fPalaceLoaded) return;
    fPalaceLoaded = true;

    loadPalaceTiles();

    {
        const auto& sds = eMun_PalaceSpriteData30;
        SpriteLoader loader(fTileH, "mun_Palace", sds,
                             nullptr, fRenderer);

        fPalace2H = loader.load(1, 1);
        fPalace1H = loader.load(1, 12);
        fPalace1W = loader.load(1, 13);
        fPalace2W = loader.load(1, 14);
    }

    {
        const auto& sds = ePalaceOverlaySpriteData30;
        SpriteLoader loader(fTileH, "palaceOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 3011; i < 3035; i++) {
            loader.load(3011, i, fPalaceHOverlay);
        }

        generateFlipped(fPalaceHOverlay, fPalaceWOverlay);
    }
}

void BuildingTextures::loadSanctuary() {
    if(fSanctuaryLoaded) return;
    fSanctuaryLoaded = true;

    {
        const auto& sds = eZeusSanctuaryElementsSpriteData30;
        SpriteLoader loader(fTileH, "zeusSanctuaryElements", sds,
                             nullptr, fRenderer);

        int i = 1;
        const auto loadStatues = [&](TextureCollection& coll) {
            for(int j = 0; j < 4; j++, i++) {
                loader.load(1, i, coll);
            }
        };
        loadStatues(fZeusStatues);
        {
            // loadStatues(fPoseidonStatues);
            i += 4;
        }
        loadStatues(fHadesStatues);
        loadStatues(fDemeterStatues);
        loadStatues(fAthenaStatues);
        loadStatues(fArtemisStatues);
        loadStatues(fApolloStatues);
        loadStatues(fAresStatues);
        loadStatues(fHephaestusStatues);
        loadStatues(fAphroditeStatues);
        loadStatues(fHermesStatues);
        loadStatues(fDionysusStatues);

        for(int i = 49; i < 55; i++) {
            loader.load(1, i, fSanctuaryTiles);
        }
        for(int i = 55; i < 61; i++) {
            loader.load(1, i, fSanctuarySpace);
        }

        fSanctuaryAltar = loader.load(1, 61);
        {
            const auto& src = fSanctuaryAltar;
            auto flipped = std::make_shared<Texture>();
            flipped->setFlipTex(src);
            flipped->setOffset(src->width() - src->offsetX(), src->offsetY());
            fSanctuaryAltarFlipped = flipped;
        }
    }

    {
        const auto& sds = ePoseidonStatues1SpriteData30;
        SpriteLoader loader(fTileH, "poseidonStatues1", sds,
                             nullptr, fRenderer);

        int i = 1;
        const auto loadStatues = [&](TextureCollection& coll) {
            for(int j = 0; j < 4; j++, i++) {
                loader.load(1, i, coll);
            }
        };

        loadStatues(fHeraStatues);
        loadStatues(fAtlasStatues);
    }
    {
        const auto& sds = ePoseidonStatues2SpriteData30;
        SpriteLoader loader(fTileH, "poseidonStatues2", sds,
                             nullptr, fRenderer);

        int i = 11;
        const auto loadStatues = [&](TextureCollection& coll) {
            for(int j = 0; j < 4; j++, i++) {
                loader.load(11, i, coll);
            }
        };

        loadStatues(fPoseidonStatues);
    }
    {
        const auto& sds = eSanctuaryFireSpriteData30;
        SpriteLoader loader(fTileH, "sanctuaryFire", sds,
                             &eSprAmbientOffset, fRenderer);


        for(int i = 2105; i < 2117; i++) {
            loader.load(2105, i, fSanctuaryFire);
        }
    }

    {
        const auto& sds = eSanctuaryOverlaySpriteData30;
        SpriteLoader loader(fTileH, "sanctuaryOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 1725; i < 1773; i++) {
            loader.load(1725, i, fSanctuaryHOverlay);
        }

        generateFlipped(fSanctuaryHOverlay, fSanctuaryWOverlay);
    }

    {
        const auto& sds = eBlankStatueSpriteData30;
        SpriteLoader loader(fTileH, "blankStatue", sds,
                             nullptr, fRenderer);

        fBlankMonument = loader.load(1, 1);
    }

    {
        const auto& sds = eZeusSanctuary1SpriteData30;
        SpriteLoader loader(fTileH, "zeusSanctuary1", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 5; i++) {
            auto& coll = fSanctuary.emplace_back(fRenderer);
            loader.load(1, i, coll);
        }

    }

    {
        const auto& sds = eZeusSanctuary2SpriteData30;
        SpriteLoader loader(fTileH, "zeusSanctuary2", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 5; i++) {
            auto& coll = fSanctuary[i - 1];
            loader.load(1, i, coll);
        }
    }
}

void BuildingTextures::loadZeusSanctuary() {
    if(fZeusSanctuaryLoaded) return;
    fZeusSanctuaryLoaded = true;

    if(fSanctuary.empty()) loadSanctuary();

    const auto& sds = eZeusSanctuary3SpriteData30;
    SpriteLoader loader(fTileH, "zeusSanctuary3", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 5; i++) {
        auto& coll = fSanctuary[i - 1];
        loader.load(1, i, coll);
    }

    // SW = SE(S3) flipped, NW overlay = NE(S3) flipped; index 2 = S3
    const auto makeFlipped = [&](const std::shared_ptr<Texture>& src,
                                 TextureCollection& dst) {
        auto& tex = dst.addTexture();
        tex->setOffset(0, src->offsetY());
        tex->setFlipTex(src);
    };
    makeFlipped(fSanctuary[1].getTexture(2), fSanctuaryFlippedSW);
    makeFlipped(fSanctuary[0].getTexture(2), fSanctuaryFlippedNW);
}

void BuildingTextures::loadHerosHall() {
    if(fHerosHallLoaded) return;
    fHerosHallLoaded = true;

    {
        const auto& sds = eZeusHerosHallSpriteData30;
        SpriteLoader loader(fTileH, "zeusHerosHall", sds,
                             nullptr, fRenderer);

        fHeroHall = loader.load(62, 62);
        for(int i = 63; i < 69; i++) {
            loader.load(62, i, fHeroStatues);
        }
    }
    {
        const auto& sds = ePoseidonHerosHallSpriteData30;
        SpriteLoader loader(fTileH, "poseidonHerosHall", sds,
                             nullptr, fRenderer);

        for(int i = 9; i < 11; i++) {
            loader.load(9, i, fHeroStatues);
        }
    }
}

void BuildingTextures::loadWaitingOverlay() {
    if(fWaitingOverlayLoaded) return;
    fWaitingOverlayLoaded = true;

    const auto& sds = eWaitingOverlaySpriteData30;
    SpriteLoader loader(fTileH, "waitingOverlay", sds,
                         &eSprAmbientOffset, fRenderer);

    loader.loadSkipFlipped(2277, 2277, 2437, fWaitingOverlay0);
    loader.loadSkipFlipped(2277, 2437, 2533, fWaitingOverlay1);
}

void BuildingTextures::loadOrangeTendersLodge() {
    if(fOrangeTendersLodgeLoaded) return;
    fOrangeTendersLodgeLoaded = true;

    const auto& sds = eOrangeTendersLodgeSpriteData30;
    SpriteLoader loader(fTileH, "orangeTendersLodge", sds,
                         nullptr, fRenderer);

    fOrangeTendersLodge = loader.load(51, 51);

    for(int i = 52; i < 87; i++) {
        loader.load(51, i, fOrangeTendersLodgeOverlay);
    }
}

void BuildingTextures::loadWaterPark() {
    if(fWaterParkLoaded) return;
    fWaterParkLoaded = true;

    const auto& sds = eWaterParkSpriteData30;
    SpriteLoader loader(fTileH, "waterPark", sds,
                         nullptr, fRenderer);

    const auto loadWaterPark = [&](const int i,
                                   std::shared_ptr<Texture>& tex,
                                   TextureCollection& overlay) {
        tex = loader.load(64, i);
        const int jMax = i + 8;
        for(int j = i + 1; j < jMax; j++) {
            loader.load(64, j, overlay);
        }
    };

    loadWaterPark(64, fWaterPark1, fWaterPark1Overlay);
    loadWaterPark(72, fWaterPark2, fWaterPark2Overlay);
    loadWaterPark(80, fWaterPark3, fWaterPark3Overlay);
    loadWaterPark(88, fWaterPark4, fWaterPark4Overlay);
    loadWaterPark(96, fWaterPark5, fWaterPark5Overlay);
    loadWaterPark(104, fWaterPark6, fWaterPark6Overlay);
    loadWaterPark(112, fWaterPark7, fWaterPark7Overlay);
    loadWaterPark(120, fWaterPark8, fWaterPark8Overlay);
}

void BuildingTextures::loadBaths() {
    if(fBathsLoaded) return;
    fBathsLoaded = true;

    const auto& sds = eBathsSpriteData30;
    SpriteLoader loader(fTileH, "baths", sds,
                         nullptr, fRenderer);

    fBaths = loader.load(36, 36);
    for(int i = 37; i < 53; i++) {
        loader.load(36, i, fBathsOverlay);
    }
}

void BuildingTextures::loadShellGarden() {
    if(fShellGardenLoaded) return;
    fShellGardenLoaded = true;


    const auto& sds = eShellGardenSpriteData30;
    SpriteLoader loader(fTileH, "shellGarden", sds,
                         nullptr, fRenderer);

    fShellGarden = loader.load(35, 35);
}

void BuildingTextures::loadOrrery() {
    if(fOrreryLoaded) return;
    fOrreryLoaded = true;

    const auto& sds = eOrrerySpriteData30;
    SpriteLoader loader(fTileH, "orrery", sds,
                         nullptr, fRenderer);

    fOrrery = loader.load(34, 34);
}

void BuildingTextures::loadDolphinSculpture() {
    if(fDolphinSculptureLoaded) return;
    fDolphinSculptureLoaded = true;

    const auto& sds = eDolphinSculptureSpriteData30;
    SpriteLoader loader(fTileH, "dolphinSculpture", sds,
                         nullptr, fRenderer);

    fDolphinSculpture = loader.load(33, 33);
}

void BuildingTextures::loadStoneCircle() {
    if(fStoneCircleLoaded) return;
    fStoneCircleLoaded = true;

    const auto& sds = eStoneCircleSpriteData30;
    SpriteLoader loader(fTileH, "stoneCircle", sds,
                         nullptr, fRenderer);

    fStoneCircle = loader.load(27, 27);
    for(int i = 28; i < 33; i++) {
        loader.load(27, i, fStoneCircleOverlay);
    }
}

void BuildingTextures::loadSpring() {
    if(fSpringLoaded) return;
    fSpringLoaded = true;

    const auto& sds = eSpringSpriteData30;
    SpriteLoader loader(fTileH, "spring", sds,
                         nullptr, fRenderer);

    for(int i = 20; i < 27; i++) {
        loader.load(20, i, fSpring);
    }
}

void BuildingTextures::loadTopiary() {
    if(fTopiaryLoaded) return;
    fTopiaryLoaded = true;

    const auto& sds = eTopiarySpriteData30;
    SpriteLoader loader(fTileH, "topiary", sds,
                         nullptr, fRenderer);

    fTopiary = loader.load(19, 19);
}

void BuildingTextures::loadSundial() {
    if(fSundialLoaded) return;
    fSundialLoaded = true;

    const auto& sds = eSundialSpriteData30;
    SpriteLoader loader(fTileH, "sundial", sds,
                         nullptr, fRenderer);

    fSundial = loader.load(18, 18);
}

void BuildingTextures::loadTallObelisk() {
    if(fTallObeliskLoaded) return;
    fTallObeliskLoaded = true;

    const auto& sds = eTallObeliskSpriteData30;
    SpriteLoader loader(fTileH, "tallObelisk", sds,
                         nullptr, fRenderer);

    fTallObelisk = loader.load(17, 17);
}

void BuildingTextures::loadAvenue() {
    if(fAvenueLoaded) return;
    fAvenueLoaded = true;

    const auto& sds = eAvenueSpriteData30;
    SpriteLoader loader(fTileH, "avenue", sds,
                         nullptr, fRenderer);

    for(int i = 39; i < 88; i++) {
        loader.load(39, i, fAvenueRoad);
    }

    fAvenue.emplace_back(fRenderer);
    for(int i = 88; i < 97; i++) {
        loader.load(39, i, fAvenue[0]);
    }

    fAvenue.emplace_back(fRenderer);
    for(int i = 97; i < 106; i++) {
        loader.load(39, i, fAvenue[1]);
    }

    fAvenue.emplace_back(fRenderer);
    for(int i = 106; i < 115; i++) {
        loader.load(39, i, fAvenue[2]);
    }

    fAvenue.emplace_back(fRenderer);
    for(int i = 115; i < 124; i++) {
        loader.load(39, i, fAvenue[3]);
    }

    fAvenue.emplace_back(fRenderer);
    for(int i = 124; i < 132; i++) {
        loader.load(39, i, fAvenue[4]);
    }
}

void BuildingTextures::loadColumns() {
    if(fColumnsLoaded) return;
    fColumnsLoaded = true;

    const auto& sds = eColumnsSpriteData30;
    SpriteLoader loader(fTileH, "columns", sds,
                         nullptr, fRenderer);

    fDoricColumn = loader.load(34, 34);
    fIonicColumn = loader.load(34, 35);
    fCorinthianColumn = loader.load(34, 36);

    fColumnConnectionH = loader.load(34, 37);
    fColumnConnectionW = loader.load(34, 38);
}

void BuildingTextures::loadCommemorative() {
    if(fCommemorativeLoaded) return;
    fCommemorativeLoaded = true;

    const auto& sds = eCommemorativeSpriteData30;
    SpriteLoader loader(fTileH, "commemorative", sds,
                         nullptr, fRenderer);

    for(int i = 25; i < 34; i++) {
        loader.load(25, i, fCommemorative);
    }
}

void BuildingTextures::loadFishPond() {
    if(fFishPondLoaded) return;
    fFishPondLoaded = true;

    {
        const auto& sds = eFishPondSpriteData30;
        SpriteLoader loader(fTileH, "fishPond", sds,
                             nullptr, fRenderer);

        fFishPond = loader.load(24, 24);
    }

    {
        const auto& sds = eFishPondOverlaySpriteData30;
        SpriteLoader loader(fTileH, "fishPondOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 2759; i < 2790; i++) {
            loader.load(2759, i, fFishPondOverlay);
        }
    }
}

void BuildingTextures::loadHedgeMaze() {
    if(fHedgeMazeLoaded) return;
    fHedgeMazeLoaded = true;

    const auto& sds = eHedgeMazeSpriteData30;
    SpriteLoader loader(fTileH, "hedgeMaze", sds,
                         nullptr, fRenderer);

    fHedgeMaze = loader.load(23, 23);
}

void BuildingTextures::loadGazebo() {
    if(fGazeboLoaded) return;
    fGazeboLoaded = true;

    const auto& sds = eGazeboSpriteData30;
    SpriteLoader loader(fTileH, "gazebo", sds,
                         nullptr, fRenderer);

    fGazebo = loader.load(22, 22);
}

void BuildingTextures::loadFlowerGarden() {
    if(fFlowerGardenLoaded) return;
    fFlowerGardenLoaded = true;

    const auto& sds = eFlowerGardenSpriteData30;
    SpriteLoader loader(fTileH, "flowerGarden", sds,
                         nullptr, fRenderer);

    fFlowerGarden = loader.load(21, 21);
}

void BuildingTextures::loadBench() {
    if(fBenchLoaded) return;
    fBenchLoaded = true;

    const auto& sds = eBenchSpriteData30;
    SpriteLoader loader(fTileH, "bench", sds,
                         nullptr, fRenderer);

    fBench = loader.load(20, 20);
}

void BuildingTextures::loadPark() {
    if(fParkLoaded) return;
    fParkLoaded = true;

    const auto& sds = eParkSpriteData30;
    SpriteLoader loader(fTileH, "park", sds,
                         nullptr, fRenderer);

    for(int i = 12; i < 16; i++) {
        loader.load(12, i, fPark);
    }
    for(int i = 16; i < 19; i++) {
        loader.load(12, i, fLargePark);
    }
    loader.load(12, 19, fHugePark);
}

void BuildingTextures::loadHippodrome() {
    if(fHippodromeLoaded) return;
    fHippodromeLoaded = true;

    const auto& sds = eHippodromeSpriteData30;
    SpriteLoader loader(fTileH, "hippodrome", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 15; i++) {
        loader.load(1, i, fHippodrome);
    }
}

void BuildingTextures::loadBirdBath() {
    if(fBirdBathLoaded) return;
    fBirdBathLoaded = true;

    const auto& sds = eBirdBathSpriteData30;
    SpriteLoader loader(fTileH, "birdBath", sds,
                         nullptr, fRenderer);

    fBirdBath = loader.load(53, 53);
    for(int i = 54; i < 63; i++) {
        loader.load(53, i, fBirdBathOverlay);
    }
}

void BuildingTextures::loadShortObelisk() {
    if(fShortObeliskLoaded) return;
    fShortObeliskLoaded = true;

    const auto& sds = eShortObeliskSpriteData30;
    SpriteLoader loader(fTileH, "shortObelisk", sds,
                         nullptr, fRenderer);

    fShortObelisk = loader.load(63, 63);
}

void BuildingTextures::loadZeusMonuments() {
    if(fZeusMonumentsLoaded) return;
    fZeusMonumentsLoaded = true;
    loadGodMonuments(eZeusStatueSpriteData30,
                     "zeusStatue",
                     fZeusMonuments);
}

void BuildingTextures::loadPoseidonMonuments() {
    if(fPoseidonMonumentsLoaded) return;
    fPoseidonMonumentsLoaded = true;
    loadGodMonuments(ePoseidonStatueSpriteData30,
                     "poseidonStatue",
                     fPoseidonMonuments);
}

void BuildingTextures::loadHadesMonuments() {
    if(fHadesMonumentsLoaded) return;
    fHadesMonumentsLoaded = true;
    loadGodMonuments(eHadesStatueSpriteData30,
                     "hadesStatue",
                     fHadesMonuments);
}

void BuildingTextures::loadDemeterMonuments() {
    if(fDemeterMonumentsLoaded) return;
    fDemeterMonumentsLoaded = true;
    loadGodMonuments(eDemeterStatueSpriteData30,
                     "demeterStatue",
                     fDemeterMonuments);
}

void BuildingTextures::loadAthenaMonuments() {
    if(fAthenaMonumentsLoaded) return;
    fAthenaMonumentsLoaded = true;
    loadGodMonuments(eAthenaStatueSpriteData30,
                     "athenaStatue",
                     fAthenaMonuments);
}

void BuildingTextures::loadArtemisMonuments() {
    if(fArtemisMonumentsLoaded) return;
    fArtemisMonumentsLoaded = true;
    loadGodMonuments(eArtemisStatueSpriteData30,
                     "artemisStatue",
                     fArtemisMonuments);
}

void BuildingTextures::loadApolloMonuments() {
    if(fApolloMonumentsLoaded) return;
    fApolloMonumentsLoaded = true;
    loadGodMonuments(eApolloStatueSpriteData30,
                     "apolloStatue",
                     fApolloMonuments);
}

void BuildingTextures::loadAresMonuments() {
    if(fAresMonumentsLoaded) return;
    fAresMonumentsLoaded = true;
    loadGodMonuments(eAresStatueSpriteData30,
                     "aresStatue",
                     fAresMonuments);
}

void BuildingTextures::loadHephaestusMonuments() {
    if(fHephaestusMonumentsLoaded) return;
    fHephaestusMonumentsLoaded = true;
    loadGodMonuments(eHephaestusStatueSpriteData30,
                     "hephaestusStatue",
                     fHephaestusMonuments);
}

void BuildingTextures::loadAphroditeMonuments() {
    if(fAphroditeMonumentsLoaded) return;
    fAphroditeMonumentsLoaded = true;
    loadGodMonuments(eAphroditeStatueSpriteData30,
                     "aphroditeStatue",
                     fAphroditeMonuments);
}

void BuildingTextures::loadHermesMonuments() {
    if(fHermesMonumentsLoaded) return;
    fHermesMonumentsLoaded = true;
    loadGodMonuments(eHermesStatueSpriteData30,
                     "hermesStatue",
                     fHermesMonuments);
}

void BuildingTextures::loadDionysusMonuments() {
    if(fDionysusMonumentsLoaded) return;
    fDionysusMonumentsLoaded = true;
    loadGodMonuments(eDionysusStatueSpriteData30,
                     "dionysusStatue",
                     fDionysusMonuments);
}

void BuildingTextures::loadAtlasMonuments() {
    if(fAtlasMonumentsLoaded) return;
    fAtlasMonumentsLoaded = true;
    loadGodMonuments(eAtlasStatueSpriteData30,
                     "atlasStatue",
                     fAtlasMonuments);
}

void BuildingTextures::loadHeraMonuments() {
    if(fHeraMonumentsLoaded) return;
    fHeraMonumentsLoaded = true;
    loadGodMonuments(eHeraStatueSpriteData30,
                     "heraStatue",
                     fHeraMonuments);
}

void BuildingTextures::loadAgora() {
    if(fAgoraLoaded) return;
    fAgoraLoaded = true;

    {
        const auto& sds = eAgoraSpriteData30;
        SpriteLoader loader(fTileH, "agora", sds,
                             nullptr, fRenderer);

        fFoodVendor = loader.load(1, 1);
        fFoodVendorOverlay = loader.load(1, 2);
        fFleeceVendor = loader.load(1, 3);
        fFleeceVendorOverlay = loader.load(1, 4);
        fOilVendor = loader.load(1, 5);
        fOilVendorOverlay = loader.load(1, 6);
        fWineVendor = loader.load(1, 7);
        fWineVendorOverlay = loader.load(1, 8);
        fArmsVendor = loader.load(1, 9);
        fArmsVendorOverlay = loader.load(1, 10);
        fHorseTrainer = loader.load(1, 11);
        fHorseTrainerOverlay = loader.load(1, 12);

        for(int i = 13; i < 16; i++) {
            loader.load(1, i, fAgoraRoad);
        }
        for(int i = 16; i < 19; i++) {
            loader.load(1, i, fAgora);
        }
    }
    {
        const auto& sds = eVendorOverlaysSpriteData30;
        SpriteLoader loader(fTileH, "vendorOverlays", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 2639; i < 2651; i++) {
            loader.load(2639, i, fFleeceVendorOverlay2);
        }

        for(int i = 2651; i < 2663; i++) {
            loader.load(2639, i, fFoodVendorOverlay2);
        }

        for(int i = 2663; i < 2679; i++) {
            loader.load(2639, i, fHorseTrainerOverlay2);
        }

        for(int i = 2679; i < 2719; i++) {
            loader.load(2639, i, fOilVendorOverlay2);
        }

        for(int i = 2719; i < 2759; i++) {
            loader.load(2639, i, fArmsVendorOverlay2);
        }
    }
}

void BuildingTextures::loadChariotVendor() {
    if(fChariotVendorLoaded) return;
    fChariotVendorLoaded = true;
    {
        const auto& sds = eChariotVendorSpriteData30;
        SpriteLoader loader(fTileH, "chariotVendor", sds,
                             nullptr, fRenderer);

        fChariotVendor = loader.load(49, 49);
        fChariotVendorOverlay = loader.load(49, 50);
    }
    {
        const auto& sds = eChariotVendorOverlaySpriteData30;
        SpriteLoader loader(fTileH, "chariotVendorOverlay", sds,
                             &ePoseidonImpsOffset, fRenderer);

        for(int i = 3674; i < 3718; i++) {
            loader.load(3674, i, fChariotVendorOverlay2);
        }
    }
}

void BuildingTextures::loadWineVendor() {
    if(fWineVendorLoaded) return;
    fWineVendorLoaded = true;
    const auto& sds = eWineVendorOverlaySpriteData30;
    SpriteLoader loader(fTileH, "wineVendorOverlay", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 429; i < 453; i++) {
        loader.load(429, i, fWineVendorOverlay2);
    }
}

void BuildingTextures::loadGranary() {
    if(fGranaryLoaded) return;
    fGranaryLoaded = true;

    const auto& sds = eGranarySpriteData30;
    SpriteLoader loader(fTileH, "granary", sds,
                         nullptr, fRenderer);

    fGranary = loader.load(29, 29);
    for(int i = 30; i < 46; i++) {
        loader.load(29, i, fGranaryOverlay);
    }

    fGranaryUrchin = loader.load(29, 46);
    fGranaryFish = loader.load(29, 47);
    fGranaryMeat = loader.load(29, 48);
    fGranaryCheese = loader.load(29, 49);
    fGranaryCheese->setOffset(-3, 2);
    fGranaryCarrots = loader.load(29, 50);
    fGranaryOnions = loader.load(29, 51);
    fGranaryOnions->setOffset(-4, 0);
    fGranaryWheat = loader.load(29, 52);
}

void BuildingTextures::loadPier() {
    if(fPierLoaded) return;
    fPierLoaded = true;

    {
        const auto& sds = ePier1SpriteData30;
        SpriteLoader loader(fTileH, "pier1", sds,
                             nullptr, fRenderer);

        for(int i = 19; i < 23; i++) {
            loader.load(19, i, fPier1);
        }
    }

    {
        const auto& sds = ePier2SpriteData30;
        SpriteLoader loader(fTileH, "pier2", sds,
                             nullptr, fRenderer);


        fPier2 = loader.load(25, 25);
        fPier2->setOffset(-88, 90);
    }

    {
        auto loadFrame = [&](const int i) -> std::shared_ptr<Texture> {
            char buf[64];
            snprintf(buf, sizeof(buf), "SprAmbient_%05d.png", i);
            const std::string path = "30/SprAmbient/" + std::string(buf);
            auto tex = BinaryImageLoader::load(fRenderer, path);
            if(tex) {
                const auto& off = eSprAmbientOffset[i - 1];
                tex->setOffset(off.first, off.second);
                return tex;
            }
            return nullptr;
        };
        // BR (0-31): mirrored BL frames (134-165)
        for(int i = 0; i < 32; i++) {
            const auto src = loadFrame(134 + i);
            auto& slot = fPierOverlay.addTexture();
            if(src) {
                auto flipped = std::make_shared<Texture>();
                flipped->setFlipTex(src);
                flipped->setOffset(src->width() - src->offsetX(),
                                   src->offsetY());
                slot = flipped;
            }
        }
        // BL (32-63): 134-165
        for(int i = 134; i < 166; i++) {
            auto& slot = fPierOverlay.addTexture();
            slot = loadFrame(i);
        }
        // TL (64-95): mirrored TR frames (198-229)
        for(int i = 0; i < 32; i++) {
            const auto src = loadFrame(198 + i);
            auto& slot = fPierOverlay.addTexture();
            if(src) {
                auto flipped = std::make_shared<Texture>();
                flipped->setFlipTex(src);
                flipped->setOffset(src->width() - src->offsetX(),
                                   src->offsetY());
                slot = flipped;
            }
        }
        // TR (96-127): 198-229
        for(int i = 198; i < 230; i++) {
            auto& slot = fPierOverlay.addTexture();
            slot = loadFrame(i);
        }

        // load/unload anim: 4 real dir blocks, 34 frames each, no mirror
        // bottomRight = S (0-33): 327-360
        for(int i = 327; i < 361; i++) {
            auto& slot = fPierLoadOverlay.addTexture();
            slot = loadFrame(i);
        }
        // bottomLeft = W (34-67): 293-326
        for(int i = 293; i < 327; i++) {
            auto& slot = fPierLoadOverlay.addTexture();
            slot = loadFrame(i);
        }
        // topLeft = N (68-101): 361-394
        for(int i = 361; i < 395; i++) {
            auto& slot = fPierLoadOverlay.addTexture();
            slot = loadFrame(i);
        }
        // topRight = E (102-135): 395-428
        for(int i = 395; i < 429; i++) {
            auto& slot = fPierLoadOverlay.addTexture();
            slot = loadFrame(i);
        }
    }
}

void BuildingTextures::loadTradingPost() {
    if(fTradingPostLoaded) return;
    fTradingPostLoaded = true;

    {
        const auto& sds = eTradingPostOverlaySpriteData30;
        SpriteLoader loader(fTileH, "tradingPostOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 43; i < 82; i++) {
            loader.load(43, i, fTradingPostOverlay);
        }
    }
    {
        const auto& sds = eTradingPostSpriteData30;
        SpriteLoader loader(fTileH, "tradingPost", sds,
                             nullptr, fRenderer);

        fTradingPost = loader.load(27, 27);
        fTradingPost->setOffset(-88, 90);
    }
}

void BuildingTextures::load() {
    if(fLoaded) return;
    fLoaded = true;

    {
        const auto& sds = eWarehouseOverlaySpriteData30;
        SpriteLoader loader(fTileH, "warehouseOverlay", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 269; i < 293; i++) {
            loader.load(269, i, fWarehouseOverlay);
        }
    }
    {
        const auto& sds = eFishSpriteData30;
        SpriteLoader loader(fTileH, "fish", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 453; i < 493; i++) {
            loader.load(453, i, fFish);
        }
    }
    {
        const auto& sds = eUrchinSpriteData30;
        SpriteLoader loader(fTileH, "urchin", sds,
                             &eSprAmbientOffset, fRenderer);

        for(int i = 3035; i < 3051; i++) {
            loader.load(3035, i, fUrchin);
        }
    }

    {
        const auto& sds = eStor_baysSpriteData30;
        SpriteLoader loader(fTileH, "stor_bays", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 5; i++) {
            loader.load(1, i, fWarehouseUrchin);
        }
        for(int i = 5; i < 9; i++) {
            loader.load(1, i, fWarehouseFish);
        }
        for(int i = 9; i < 13; i++) {
            loader.load(1, i, fWarehouseMeat);
        }
        for(int i = 13; i < 17; i++) {
            loader.load(1, i, fWarehouseCheese);
        }
        for(int i = 17; i < 21; i++) {
            loader.load(1, i, fWarehouseCarrots);
        }
        for(int i = 21; i < 25; i++) {
            loader.load(1, i, fWarehouseOnions);
        }
        for(int i = 25; i < 29; i++) {
            loader.load(1, i, fWarehouseWheat);
        }
        for(int i = 29; i < 33; i++) {
            loader.load(1, i, fWarehouseWood);
        }
        for(int i = 33; i < 37; i++) {
            loader.load(1, i, fWarehouseBronze);
        }
        for(int i = 37; i < 41; i++) {
            loader.load(1, i, fWarehouseMarble);
        }
        for(int i = 41; i < 45; i++) {
            loader.load(1, i, fWarehouseGrapes);
        }
        for(int i = 45; i < 49; i++) {
            loader.load(1, i, fWarehouseOlives);
        }
        for(int i = 49; i < 53; i++) {
            loader.load(1, i, fWarehouseFleece);
        }
        for(int i = 53; i < 57; i++) {
            loader.load(1, i, fWarehouseArmor);
        }
        fWarehouseSculpture = loader.load(1, 57);
        for(int i = 58; i < 62; i++) {
            loader.load(1, i, fWarehouseOliveOil);
        }
        for(int i = 62; i < 66; i++) {
            loader.load(1, i, fWarehouseWine);
        }


        for(int i = 71; i < 76; i++) {
            loader.load(1, i, fWaitingMeat);
        }
        for(int i = 76; i < 81; i++) {
            loader.load(1, i, fWaitingCheese);
        }
        for(int i = 81; i < 89; i++) {
            loader.load(1, i, fWaitingWheat);
        }
        for(int i = 89; i < 94; i++) {
            loader.load(1, i, fWaitingWood);
        }
        for(int i = 94; i < 99; i++) {
            loader.load(1, i, fWaitingBronze);
        }
        for(int i = 99; i < 102; i++) {
            loader.load(1, i, fWaitingGrapes);
        }
        for(int i = 102; i < 105; i++) {
            loader.load(1, i, fWaitingOlives);
        }
        for(int i = 105; i < 110; i++) {
            loader.load(1, i, fWaitingArmor);
        }

        fWarehouseEmpty = loader.load(1, 115);
    }

    {
        const auto& sds = eWarehouseSpriteData30;
        SpriteLoader loader(fTileH, "warehouse", sds,
                             nullptr, fRenderer);

        fWarehouseDoor = loader.load(23, 23);
        fWarehouseDoor->setOffset(-58, 60);
    }

    {
        const auto& sds = eStorageSpriteData30;
        SpriteLoader loader(fTileH, "storage", sds,
                             nullptr, fRenderer);

        for(int i = 87; i < 91; i++) {
            loader.load(87, i, fWarehouseOranges);
        }

        for(int i = 91; i < 95; i++) {
            loader.load(87, i, fWarehouseBlackMarble);
        }

        for(int i = 95; i < 99; i++) {
            loader.load(87, i, fWarehouseOrichalc);
        }

        for(int i = 99; i < 104; i++) {
            loader.load(87, i, fWaitingOranges);
        }

        for(int i = 104; i < 109; i++) {
            loader.load(87, i, fWaitingOrichalc);
        }

        for(int i = 110; i < 114; i++) {
            loader.load(87, i, fChariotFactoryChariots);
        }

        fGranaryOranges = loader.load(87, 122);
        fGranaryOranges->setOffset(-3, -3);

        for(int i = 123; i < 129; i++) {
            loader.load(87, i, fOrangeTree);
        }
    }


    {
        const auto& sds = eInterfaceSpritesSpriteData30;
        SpriteLoader loader(fTileH, "interfaceSprites", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 4; i++) {
            loader.load(1, i, fColumn1);
        }
        for(int i = 4; i < 7; i++) {
            loader.load(1, i, fColumn2);
        }
        for(int i = 7; i < 10; i++) {
            loader.load(1, i, fColumn3);
        }
        for(int i = 10; i < 13; i++) {
            loader.load(1, i, fColumn4);
        }
        for(int i = 13; i < 16; i++) {
            loader.load(1, i, fColumn5);
        }

        for(int i = 20; i < 28; i++) {
            loader.load(1, i, fSpawner);
        }
        fDisasterPoint = loader.load(1, 28);
        fLandInvasionPoint = loader.load(1, 29);
        fEntryPoint = loader.load(1, 30);
        fExitPoint = loader.load(1, 31);
        fRiverEntryPoint = loader.load(1, 32);
        fRiverExitPoint = loader.load(1, 33);
        fFishPoint = loader.load(1, 34);
        fUrchinPoint = loader.load(1, 35);
        fWolfPoint = loader.load(1, 36);
        fBoarPoint = loader.load(1, 37);
        fMonsterPoint = loader.load(1, 38);
        fDisembarkPoint = loader.load(1, 39);
    }


    {
        const auto& sds = eDeerTopSpriteData30;
        SpriteLoader loader(fTileH, "deerTop", sds,
                             nullptr, fRenderer);

        fDeerPoint = loader.load(57, 57);
    }


    {
        const auto& sds = eSuppliesSpriteData30;
        SpriteLoader loader(fTileH, "supplies", sds,
                             nullptr, fRenderer);

        fSuppliesBg = loader.load(137, 137);
        fEliteSuppliesBg = loader.load(137, 139);

        fHasFood = loader.load(137, 155);
        fHasFleece = loader.load(137, 153);
        fHasOil = loader.load(137, 156);
        fHasWine = loader.load(137, 157);
        fHasArms = loader.load(137, 154);
        fHasHorses = loader.load(137, 158);

        fNHasFood = loader.load(137, 161);
        fNHasFleece = loader.load(137, 159);
        fNHasOil = loader.load(137, 162);
        fNHasWine = loader.load(137, 163);
        fNHasArms = loader.load(137, 160);
        fNHasHorses = loader.load(137, 164);
    }
}

void BuildingTextures::loadGodMonuments(
        const std::vector<eSpriteData>& sd,
        const std::string& name,
        TextureCollection& coll) {
    SpriteLoader loader(fTileH, name, sd, nullptr, fRenderer);
    for(int i = 1; i < 5; i++) {
        loader.load(1, i, coll);
    }
}

void BuildingTextures::generateFlipped(const TextureCollection& src,
                                        TextureCollection& dst) {
    for(int i = 0; i < src.size(); i++) {
        auto& tex = dst.addTexture();
        const auto& srcTex = src.getTexture(i);
        const int w = srcTex->width();
        const int ox = srcTex->offsetX();
        const int oy = srcTex->offsetY();
        tex->setOffset(w - ox, oy);
        tex->setFlipTex(srcTex);
    }
}
