#ifndef BUILDING_TEXTURES_H
#define BUILDING_TEXTURES_H

#include "texture-collection.h"

struct eSpriteData;

class BuildingTextures {
public:
    BuildingTextures(const int tileW, const int tileH,
                     SDL_Renderer* const renderer);

    void loadAll();

    void load();
    bool fLoaded = false;

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    bool fCommonHouseLoaded = false;
    void loadCommonHouse();
    bool fPoseidonCommonHouseLoaded = false;
    void loadPoseidonCommonHouse();
    bool fEliteHouseLoaded = false;
    void loadEliteHouse();
    bool fPoseidonEliteHouseLoaded = false;
    void loadPoseidonEliteHouse();
    bool fCollegeLoaded = false;
    void loadCollege();
    bool fGymnasiumLoaded = false;
    void loadGymnasium();
    bool fDramaSchoolLoaded = false;
    void loadDramaSchool();
    bool fPodiumLoaded = false;
    void loadPodium();
    bool fTheaterLoaded = false;
    void loadTheater();
    bool fStadiumLoaded = false;
    void loadStadium();
    bool fFountainLoaded = false;
    void loadFountain();
    bool fHospitalLoaded = false;
    void loadHospital();
    bool fOliveTreeLoaded = false;
    void loadOliveTree();
    bool fVineLoaded = false;
    void loadVine();
    bool fPlantationLoaded = false;
    void loadPlantation();
    bool fHuntingLodgeLoaded = false;
    void loadHuntingLodge();
    bool fFisheryLoaded = false;
    void loadFishery();
    bool fUrchinQuayLoaded = false;
    void loadUrchinQuay();
    bool fCardingShedLoaded = false;
    void loadCardingShed();
    bool fDairyLoaded = false;
    void loadDairy();
    bool fGrowersLodgeLoaded = false;
    void loadGrowersLodge();
    bool fTimberMillLoaded = false;
    void loadTimberMill();
    bool fMasonryShopLoaded = false;
    void loadMasonryShop();
    bool fBlackMarbleWorkshopLoaded = false;
    void loadBlackMarbleWorkshop();
    bool fRefineryLoaded = false;
    void loadRefinery();
    bool fOrichalcTowerOverlayLoaded = false;
    void loadOrichalcTowerOverlay();
    bool fMasonryShopOverlaysLoaded = false;
    void loadMasonryShopOverlays();
    bool fMintLoaded = false;
    void loadMint();
    bool fFoundryLoaded = false;
    void loadFoundry();
    bool fArtisansGuildLoaded = false;
    void loadArtisansGuild();
    bool fOlivePressLoaded = false;
    void loadOlivePress();
    bool fWineryLoaded = false;
    void loadWinery();
    bool fSculptureStudioLoaded = false;
    void loadSculptureStudio();
    bool fTriremeWharfLoaded = false;
    void loadTriremeWharf();
    bool fHorseRanchLoaded = false;
    void loadHorseRanch();
    bool fArmoryLoaded = false;
    void loadArmory();
    bool fGatehouseAndTowerLoaded = false;
    void loadGatehouseAndTower();
    bool fWallLoaded = false;
    void loadWall();
    bool fMaintenanceOfficeLoaded = false;
    void loadMaintenanceOffice();
    bool fTaxOfficeLoaded = false;
    void loadTaxOffice();
    bool fWatchpostLoaded = false;
    void loadWatchpost();
    bool fRoadblockLoaded = false;
    void loadRoadblock();
    bool fBridgeLoaded = false;
    void loadBridge();
    bool fPoseidonBridgeLoaded = false;
    void loadPoseidonBridge();
    bool fPalaceLoaded = false;
    void loadPalace();
    bool fPalaceTilesLoaded = false;
    void loadPalaceTiles();
    bool fSanctuaryLoaded = false;
    void loadSanctuary();
    bool fZeusSanctuaryLoaded = false;
    void loadZeusSanctuary();
    bool fPoseidonSanctuaryLoaded = false;
    void loadPoseidonSanctuary();
    bool fPyramidLoaded = false;
    void loadPyramid();
    bool fPyramid2Loaded = false;
    void loadPyramid2();
    bool fPoseidonHerosHallLoaded = false;
    void loadPoseidonHerosHall();
    bool fHerosHallLoaded = false;
    void loadHerosHall();
    bool fWaitingOverlayLoaded = false;
    void loadWaitingOverlay();
    bool fOrangeTendersLodgeLoaded = false;
    void loadOrangeTendersLodge();
    bool fWaterParkLoaded = false;
    void loadWaterPark();
    bool fShortObeliskLoaded = false;
    void loadShortObelisk();
    bool fBirdBathLoaded = false;
    void loadBirdBath();
    bool fBathsLoaded = false;
    void loadBaths();
    bool fShellGardenLoaded = false;
    void loadShellGarden();
    bool fOrreryLoaded = false;
    void loadOrrery();
    bool fDolphinSculptureLoaded = false;
    void loadDolphinSculpture();
    bool fStoneCircleLoaded = false;
    void loadStoneCircle();
    bool fSpringLoaded = false;
    void loadSpring();
    bool fTopiaryLoaded = false;
    void loadTopiary();
    bool fSundialLoaded = false;
    void loadSundial();
    bool fTallObeliskLoaded = false;
    void loadTallObelisk();
    bool fAvenueLoaded = false;
    void loadAvenue();
    bool fColumnsLoaded = false;
    void loadColumns();
    bool fCommemorativeLoaded = false;
    void loadCommemorative();
    bool fFishPondLoaded = false;
    void loadFishPond();
    bool fHedgeMazeLoaded = false;
    void loadHedgeMaze();
    bool fGazeboLoaded = false;
    void loadGazebo();
    bool fFlowerGardenLoaded = false;
    void loadFlowerGarden();
    bool fBenchLoaded = false;
    void loadBench();
    bool fParkLoaded = false;
    void loadPark();
    bool fHippodromeLoaded = false;
    void loadHippodrome();

    bool fZeusMonumentsLoaded = false;
    void loadZeusMonuments();
    bool fPoseidonMonumentsLoaded = false;
    void loadPoseidonMonuments();
    bool fHadesMonumentsLoaded = false;
    void loadHadesMonuments();
    bool fDemeterMonumentsLoaded = false;
    void loadDemeterMonuments();
    bool fAthenaMonumentsLoaded = false;
    void loadAthenaMonuments();
    bool fArtemisMonumentsLoaded = false;
    void loadArtemisMonuments();
    bool fApolloMonumentsLoaded = false;
    void loadApolloMonuments();
    bool fAresMonumentsLoaded = false;
    void loadAresMonuments();
    bool fHephaestusMonumentsLoaded = false;
    void loadHephaestusMonuments();
    bool fAphroditeMonumentsLoaded = false;
    void loadAphroditeMonuments();
    bool fHermesMonumentsLoaded = false;
    void loadHermesMonuments();
    bool fDionysusMonumentsLoaded = false;
    void loadDionysusMonuments();

    bool fHeraMonumentsLoaded = false;
    void loadHeraMonuments();
    bool fAtlasMonumentsLoaded = false;
    void loadAtlasMonuments();

    bool fAgoraLoaded = false;
    void loadAgora();
    bool fGranaryLoaded = false;
    void loadGranary();
    bool fPierLoaded = false;
    void loadPier();
    bool fTradingPostLoaded = false;
    void loadTradingPost();

    bool fBibliothekeLoaded = false;
    void loadBibliotheke();
    bool fObservatoryLoaded = false;
    void loadObservatory();
    bool fUniversityLoaded = false;
    void loadUniversity();
    bool fLaboratoryLoaded = false;
    void loadLaboratory();
    bool fInventorsWorkshopLoaded = false;
    void loadInventorsWorkshop();
    bool fMuseumLoaded = false;
    void loadMuseum();

    bool fCorralLoaded = false;
    void loadCorral();

    bool fChariotFactoryLoaded = false;
    void loadChariotFactory();

    bool fChariotVendorLoaded = false;
    void loadChariotVendor();

    bool fWineVendorLoaded = false;
    void loadWineVendor();

    bool fAltarBullOverlayLoaded = false;
    void loadAltarBullOverlay();
    bool fAltarSheepOverlayLoaded = false;
    void loadAltarSheepOverlay();
    bool fAltarGoodsOverlayLoaded = false;
    void loadAltarGoodsOverlay();

    bool fHippodromeSpectatorsLoaded = false;
    void loadHippodromeSpectators();

    bool fHippodromeFecesLoaded = false;
    void loadHippodromeFeces();

    bool fHippodromeFinishLoaded = false;
    void loadHippodromeFinish();

    std::shared_ptr<Texture> fHouseSpace;
    std::vector<TextureCollection> fCommonHouse;
    std::vector<TextureCollection> fPoseidonCommonHouse;

    std::vector<TextureCollection> fEliteHouse;
    std::vector<TextureCollection> fPoseidonEliteHouse;
    TextureCollection fEliteHouseHorses;

    std::shared_ptr<Texture> fGymnasium;
    TextureCollection fGymnasiumOverlay;

    std::shared_ptr<Texture> fCollege;
    TextureCollection fCollegeOverlay;

    std::shared_ptr<Texture> fDramaSchool;
    TextureCollection fDramaSchoolOverlay;

    std::shared_ptr<Texture> fPodium;
    TextureCollection fPodiumOverlay;

    std::shared_ptr<Texture> fTheater;
    TextureCollection fTheaterOverlay;

    std::shared_ptr<Texture> fStadium1H;
    std::shared_ptr<Texture> fStadium2H;

    std::shared_ptr<Texture> fStadium1W;
    std::shared_ptr<Texture> fStadium2W;

    TextureCollection fStadiumOverlay1;
    TextureCollection fStadiumOverlay2;
    TextureCollection fStadiumOverlay3;
    TextureCollection fStadiumOverlay4W;
    TextureCollection fStadiumOverlay4H;
    TextureCollection fStadiumOverlay5W;
    TextureCollection fStadiumOverlay5H;

    TextureCollection fStadiumAudiance1W;
    TextureCollection fStadiumAudiance2W;

    TextureCollection fStadiumAudiance1H;
    TextureCollection fStadiumAudiance2H;

    std::shared_ptr<Texture> fBibliotheke;
    TextureCollection fBibliothekeOverlay;

    std::shared_ptr<Texture> fObservatory;
    TextureCollection fObservatoryOverlay;

    std::shared_ptr<Texture> fUniversity;
    TextureCollection fUniversityOverlay;

    std::shared_ptr<Texture> fLaboratory;
    TextureCollection fLaboratoryOverlay;

    std::shared_ptr<Texture> fInventorsWorkshop;
    TextureCollection fInventorsWorkshopOverlay;

    std::shared_ptr<Texture> fMuseum;
    TextureCollection fMuseumOverlay;

    std::shared_ptr<Texture> fPalace1H;
    std::shared_ptr<Texture> fPalace2H;

    std::shared_ptr<Texture> fPalace1W;
    std::shared_ptr<Texture> fPalace2W;

    TextureCollection fPalaceHOverlay;
    TextureCollection fPalaceWOverlay;

    TextureCollection fPalaceTiles;

    std::shared_ptr<Texture> fFountain;
    TextureCollection fFountainOverlay;

    std::shared_ptr<Texture> fHospital;
    TextureCollection fHospitalOverlay;


    TextureCollection fOliveTree;
    TextureCollection fVine;
    TextureCollection fOrangeTree;

    std::shared_ptr<Texture> fPlantation;
    TextureCollection fWheat;
    TextureCollection fCarrots;
    TextureCollection fOnions;

    std::shared_ptr<Texture> fHuntingLodge;
    TextureCollection fHuntingLodgeOverlay;

    TextureCollection fFishery;
    TextureCollection fFisheryBoatBuildingW;
    TextureCollection fFisheryBoatBuildingH;
    std::vector<TextureCollection> fFisheryOverlay;
    TextureCollection fFisheryUnpackingOverlayTL;
    TextureCollection fFisheryUnpackingOverlayTR;
    TextureCollection fFisheryUnpackingOverlayBL;
    TextureCollection fFisheryUnpackingOverlayBR;
    TextureCollection fUrchinQuay;
    TextureCollection fUrchinQuayUnpackingOverlayTL;
    TextureCollection fUrchinQuayUnpackingOverlayTR;
    TextureCollection fUrchinQuayUnpackingOverlayBL;
    TextureCollection fUrchinQuayUnpackingOverlayBR;

    std::shared_ptr<Texture> fCardingShed;
    TextureCollection fCardingShedOverlay;

    std::shared_ptr<Texture> fDairy;
    TextureCollection fDairyOverlay;

    std::shared_ptr<Texture> fGrowersLodge;
    TextureCollection fGrowersLodgeOverlay;

//    std::shared_ptr<Texture> fCorral;
//    TextureCollection fCorralOverlay;

    std::shared_ptr<Texture> fOrangeTendersLodge;
    TextureCollection fOrangeTendersLodgeOverlay;

    std::shared_ptr<Texture> fTimberMill;
    TextureCollection fTimberMillOverlay;

    std::shared_ptr<Texture> fMasonryShop;
    TextureCollection fMasonryShopStones;
    std::shared_ptr<Texture> fBlackMarbleWorkshop;
    TextureCollection fBlackMarbleWorkshopStones;
    std::vector<TextureCollection> fWaitingOverlay0;
    std::vector<TextureCollection> fWaitingOverlay1;
    std::vector<TextureCollection> fMasonryShopOverlay1;
    std::vector<TextureCollection> fMasonryShopOverlay2;

    std::shared_ptr<Texture> fRefinery;
    TextureCollection fRefineryOverlay;

    std::shared_ptr<Texture> fOrichalcTowerOverlay;

    std::shared_ptr<Texture> fMint;
    TextureCollection fMintOverlay;

    std::shared_ptr<Texture> fFoundry;
    TextureCollection fFoundryOverlay;

    std::shared_ptr<Texture> fArtisansGuild;
    TextureCollection fArtisansGuildOverlay;

    std::shared_ptr<Texture> fOlivePress;
    TextureCollection fOlivePressOverlay;

    std::shared_ptr<Texture> fWinery;
    TextureCollection fWineryOverlay;

    std::shared_ptr<Texture> fSculptureStudio;
    TextureCollection fSculptureStudioOverlay;

    TextureCollection fTriremeWharf;
    TextureCollection fTriremeWharfOverlay1BL;
    TextureCollection fTriremeWharfOverlay1BR;
    TextureCollection fTriremeWharfOverlay1TL;
    TextureCollection fTriremeWharfOverlay1TR;
    TextureCollection fTriremeWharfOverlay2BL;
    TextureCollection fTriremeWharfOverlay2BR;
    TextureCollection fTriremeWharfOverlay2TL;
    TextureCollection fTriremeWharfOverlay2TR;

    std::shared_ptr<Texture> fHorseRanch;
    TextureCollection fHorseRanchOverlay;
    std::shared_ptr<Texture> fHorseRanchEnclosure;

    std::shared_ptr<Texture> fCorral;
    TextureCollection fCorralOverlay;
    std::vector<TextureCollection> fCorralCattleOverlay;
    TextureCollection fCorralProcessingOverlay;

    std::shared_ptr<Texture> fArmory;
    TextureCollection fArmoryOverlay;

    TextureCollection fGatehouseW;
    TextureCollection fGatehouseH;
    std::shared_ptr<Texture> fTower;
    TextureCollection fWall;

    std::shared_ptr<Texture> fMaintenanceOffice;
    TextureCollection fMaintenanceOfficeOverlay;

    std::shared_ptr<Texture> fTaxOffice;
    TextureCollection fTaxOfficeOverlay;

    std::shared_ptr<Texture> fWatchPost;
    TextureCollection fWatchPostOverlay;

    TextureCollection fWaitingMeat;
    TextureCollection fWaitingCheese;
    TextureCollection fWaitingWheat;
    TextureCollection fWaitingOranges;
    TextureCollection fWaitingWood;
    TextureCollection fWaitingBronze;
    TextureCollection fWaitingGrapes;
    TextureCollection fWaitingOlives;
    TextureCollection fWaitingArmor;
    TextureCollection fWaitingOrichalc;

    TextureCollection fAgoraRoad;
    TextureCollection fAgora;

    std::shared_ptr<Texture> fFoodVendor;
    std::shared_ptr<Texture> fFoodVendorOverlay;
    TextureCollection fFoodVendorOverlay2;

    std::shared_ptr<Texture> fFleeceVendor;
    std::shared_ptr<Texture> fFleeceVendorOverlay;
    TextureCollection fFleeceVendorOverlay2;

    std::shared_ptr<Texture> fOilVendor;
    std::shared_ptr<Texture> fOilVendorOverlay;
    TextureCollection fOilVendorOverlay2;

    std::shared_ptr<Texture> fArmsVendor;
    std::shared_ptr<Texture> fArmsVendorOverlay;
    TextureCollection fArmsVendorOverlay2;

    std::shared_ptr<Texture> fWineVendor;
    std::shared_ptr<Texture> fWineVendorOverlay;
    TextureCollection fWineVendorOverlay2;

    std::shared_ptr<Texture> fHorseTrainer;
    std::shared_ptr<Texture> fHorseTrainerOverlay;
    TextureCollection fHorseTrainerOverlay2;

    std::shared_ptr<Texture> fChariotVendor;
    std::shared_ptr<Texture> fChariotVendorOverlay;
    TextureCollection fChariotVendorOverlay2;

    std::shared_ptr<Texture> fChariotFactory;
    TextureCollection fChariotFactoryOverlay;
    TextureCollection fChariotFactoryOverlay1;
    std::vector<TextureCollection> fChariotFactoryHorses;
    TextureCollection fChariotFactoryChariots;

    std::shared_ptr<Texture> fWarehouseDoor;
    TextureCollection fWarehouseOverlay;

    TextureCollection fPier1;
    std::shared_ptr<Texture> fPier2;
    TextureCollection fPierOverlay;
    TextureCollection fPierLoadOverlay;

    std::shared_ptr<Texture> fTradingPost;
    TextureCollection fTradingPostOverlay;

    std::shared_ptr<Texture> fWarehouseEmpty;
    TextureCollection fWarehouseUrchin;
    TextureCollection fWarehouseFish;
    TextureCollection fWarehouseMeat;
    TextureCollection fWarehouseCheese;
    TextureCollection fWarehouseCarrots;
    TextureCollection fWarehouseOnions;
    TextureCollection fWarehouseWheat;
    TextureCollection fWarehouseOranges;
    TextureCollection fWarehouseBlackMarble;
    TextureCollection fWarehouseOrichalc;

    TextureCollection fWarehouseWood;
    TextureCollection fWarehouseBronze;
    TextureCollection fWarehouseMarble;
    TextureCollection fWarehouseGrapes;
    TextureCollection fWarehouseOlives;
    TextureCollection fWarehouseFleece;
    TextureCollection fWarehouseArmor;
    std::shared_ptr<Texture> fWarehouseSculpture;
    TextureCollection fWarehouseOliveOil;
    TextureCollection fWarehouseWine;

    std::shared_ptr<Texture> fGranary;
    TextureCollection fGranaryOverlay;

    std::shared_ptr<Texture> fGranaryUrchin;
    std::shared_ptr<Texture> fGranaryFish;
    std::shared_ptr<Texture> fGranaryMeat;
    std::shared_ptr<Texture> fGranaryCheese;
    std::shared_ptr<Texture> fGranaryCarrots;
    std::shared_ptr<Texture> fGranaryOnions;
    std::shared_ptr<Texture> fGranaryWheat;
    std::shared_ptr<Texture> fGranaryOranges;

    TextureCollection fPark;
    TextureCollection fLargePark;
    TextureCollection fHugePark;

    std::shared_ptr<Texture> fWaterPark1;
    TextureCollection fWaterPark1Overlay;

    std::shared_ptr<Texture> fWaterPark2;
    TextureCollection fWaterPark2Overlay;

    std::shared_ptr<Texture> fWaterPark3;
    TextureCollection fWaterPark3Overlay;

    std::shared_ptr<Texture> fWaterPark4;
    TextureCollection fWaterPark4Overlay;

    std::shared_ptr<Texture> fWaterPark5;
    TextureCollection fWaterPark5Overlay;

    std::shared_ptr<Texture> fWaterPark6;
    TextureCollection fWaterPark6Overlay;

    std::shared_ptr<Texture> fWaterPark7;
    TextureCollection fWaterPark7Overlay;

    std::shared_ptr<Texture> fWaterPark8;
    TextureCollection fWaterPark8Overlay;

    std::shared_ptr<Texture> fBench;
    std::shared_ptr<Texture> fFlowerGarden;
    std::shared_ptr<Texture> fGazebo;
    std::shared_ptr<Texture> fHedgeMaze;
    std::shared_ptr<Texture> fFishPond;
    TextureCollection fFishPondOverlay;

    std::shared_ptr<Texture> fDoricColumn;
    std::shared_ptr<Texture> fIonicColumn;
    std::shared_ptr<Texture> fCorinthianColumn;

    std::vector<TextureCollection> fAvenue;
    TextureCollection fAvenueRoad;

    TextureCollection fCommemorative;

    std::shared_ptr<Texture> fColumnConnectionH;
    std::shared_ptr<Texture> fColumnConnectionW;

    std::shared_ptr<Texture> fBirdBath;
    TextureCollection fBirdBathOverlay;

    std::shared_ptr<Texture> fShortObelisk;
    std::shared_ptr<Texture> fTallObelisk;
    std::shared_ptr<Texture> fShellGarden;
    std::shared_ptr<Texture> fOrrery;
    std::shared_ptr<Texture> fSundial;
    std::shared_ptr<Texture> fDolphinSculpture;

    TextureCollection fSpring;

    std::shared_ptr<Texture> fTopiary;

    std::shared_ptr<Texture> fBaths;
    TextureCollection fBathsOverlay;

    std::shared_ptr<Texture> fStoneCircle;
    TextureCollection fStoneCircleOverlay;

    TextureCollection fZeusStatues;
    TextureCollection fPoseidonStatues;
    TextureCollection fHadesStatues;
    TextureCollection fDemeterStatues;
    TextureCollection fAthenaStatues;
    TextureCollection fArtemisStatues;
    TextureCollection fApolloStatues;
    TextureCollection fAresStatues;
    TextureCollection fHephaestusStatues;
    TextureCollection fAphroditeStatues;
    TextureCollection fHermesStatues;
    TextureCollection fDionysusStatues;

    TextureCollection fHeraStatues;
    TextureCollection fAtlasStatues;

    TextureCollection fSanctuaryTiles;
    TextureCollection fSanctuarySpace;
    std::shared_ptr<Texture> fSanctuaryAltar;
    std::shared_ptr<Texture> fSanctuaryAltarFlipped;
    TextureCollection fAltarBullOverlay;
    TextureCollection fAltarSheepOverlay;
    TextureCollection fAltarGoodsOverlay;
    TextureCollection fAltarBullOverlayFlipped;
    TextureCollection fAltarSheepOverlayFlipped;
    TextureCollection fAltarGoodsOverlayFlipped;

    std::shared_ptr<Texture> fHeroHall;
    std::shared_ptr<Texture> fPoseidonHeroHall;
    // achilles, hercules, jason, odysseus,
    // perseus, theseus, bellerophon, atalanta
    TextureCollection fHeroStatues;

    std::shared_ptr<Texture> fBlankMonument;

    TextureCollection fZeusMonuments;
    TextureCollection fPoseidonMonuments;
    TextureCollection fHadesMonuments;
    TextureCollection fDemeterMonuments;
    TextureCollection fAthenaMonuments;
    TextureCollection fArtemisMonuments;
    TextureCollection fApolloMonuments;
    TextureCollection fAresMonuments;
    TextureCollection fHephaestusMonuments;
    TextureCollection fAphroditeMonuments;
    TextureCollection fHermesMonuments;
    TextureCollection fDionysusMonuments;

    TextureCollection fHeraMonuments;
    TextureCollection fAtlasMonuments;

    std::vector<TextureCollection> fSanctuary;
    TextureCollection fSanctuaryFlippedSW;
    TextureCollection fSanctuaryFlippedNW;
    TextureCollection fPoseidonSanctuary;
    TextureCollection fPyramid;
    TextureCollection fPyramid2;

    TextureCollection fSanctuaryWOverlay;
    TextureCollection fSanctuaryHOverlay;

    TextureCollection fSanctuaryFire;

    TextureCollection fFish;
    TextureCollection fUrchin;

    std::shared_ptr<Texture> fRoadblock;
    TextureCollection fBridge;
    TextureCollection fPoseidonBridge;

    TextureCollection fColumn1;
    TextureCollection fColumn2;
    TextureCollection fColumn3;
    TextureCollection fColumn4;
    TextureCollection fColumn5;

    TextureCollection fSpawner;
    std::shared_ptr<Texture> fDisasterPoint;
    std::shared_ptr<Texture> fLandInvasionPoint;
    std::shared_ptr<Texture> fEntryPoint;
    std::shared_ptr<Texture> fExitPoint;
    std::shared_ptr<Texture> fRiverEntryPoint;
    std::shared_ptr<Texture> fRiverExitPoint;
    std::shared_ptr<Texture> fFishPoint;
    std::shared_ptr<Texture> fUrchinPoint;
    std::shared_ptr<Texture> fWolfPoint;
    std::shared_ptr<Texture> fBoarPoint;
    std::shared_ptr<Texture> fDeerPoint;
    std::shared_ptr<Texture> fMonsterPoint;
    std::shared_ptr<Texture> fDisembarkPoint;

    std::shared_ptr<Texture> fSuppliesBg;
    std::shared_ptr<Texture> fEliteSuppliesBg;

    std::shared_ptr<Texture> fHasFood;
    std::shared_ptr<Texture> fHasFleece;
    std::shared_ptr<Texture> fHasOil;
    std::shared_ptr<Texture> fHasWine;
    std::shared_ptr<Texture> fHasArms;
    std::shared_ptr<Texture> fHasHorses;

    std::shared_ptr<Texture> fNHasFood;
    std::shared_ptr<Texture> fNHasFleece;
    std::shared_ptr<Texture> fNHasOil;
    std::shared_ptr<Texture> fNHasWine;
    std::shared_ptr<Texture> fNHasArms;
    std::shared_ptr<Texture> fNHasHorses;

    TextureCollection fHippodrome;
    std::vector<TextureCollection> fHippodromeSpectators;
    std::vector<TextureCollection> fHippodromeFeces;
    std::vector<TextureCollection> fHippodromeFecesStanding;
    std::vector<TextureCollection> fHippodromeFinishRacing;
    std::vector<TextureCollection> fHippodromeFinishNotRacing;
private:
    bool fFisheryOverlayLoaded = false;
    void loadFisheryOverlay();

    void loadGodMonuments(const std::vector<eSpriteData>& sd,
                          const std::string& name,
                          TextureCollection& coll);
    void generateFlipped(const TextureCollection& src,
                         TextureCollection& dst);
};

#endif // BUILDING_TEXTURES_H
