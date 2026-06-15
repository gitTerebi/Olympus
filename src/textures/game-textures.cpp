#include "game-textures.h"

#include <functional>
#include <filesystem>

#include "game-dir.h"

bool GameTextures::sInitialized = false;
std::vector<TerrainTextures> GameTextures::sTerrainTextures;
std::vector<GodTextures> GameTextures::sGodTextures;
std::vector<BuildingTextures> GameTextures::sBuildingTextures;
std::vector<CharacterTextures> GameTextures::sCharacterTextures;
std::vector<InterfaceTextures> GameTextures::sInterfaceTextures;
std::vector<DestructionTextures> GameTextures::sDestructionTextures;
Settings GameTextures::sSettings;

struct eLoader {
    using eFunc = std::function<void(std::string&)>;
    eLoader(const eFunc& f, const int s) :
        fFunc(f), fSize(s) {}

    eFunc fFunc;
    int fSize;
    bool fFinished = false;
};

std::vector<eLoader> gMenuLoaders;
std::vector<eLoader> gGameLoaders;

void GameTextures::loadTexture(const std::function<void(int)>& func) {
    for(int i = 0; i < 4; i++) {
        if(i == 0 && !sSettings.fTinyTextures) continue;
        if(i == 1 && !sSettings.fSmallTextures) continue;
        if(i == 2 && !sSettings.fMediumTextures) continue;
        if(i == 3 && !sSettings.fLargeTextures) continue;
        func(i);
    }
}

void GameTextures::loadInterfaceTexture(const std::function<void(int)>& func) {
    const auto scale = sSettings.fUiScale;
    for(int i = 0; i < 4; i++) {
        if(i == 0 && scale != eUIScale::tiny) continue;
        if(i == 1 && scale != eUIScale::small) continue;
        if(i == 2 && scale != eUIScale::medium) continue;
        if(i == 3 && scale != eUIScale::large) continue;
        func(i);
    }
}

void GameTextures::loadPriest() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPriest();
    });
}

void GameTextures::loadPeddler() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPeddler();
    });
}

void GameTextures::loadActor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadActor();
    });
}

void GameTextures::loadOx() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOx();
    });
}

void GameTextures::loadPorter() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPorter();
    });
}

void GameTextures::loadOxHandler() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOxHandler();
    });
}

void GameTextures::loadTrailer() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrailer();
    });
}

void GameTextures::loadSettlers() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSettlers();
    });
}

void GameTextures::loadHomeless() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHomeless();
    });
}

void GameTextures::loadFireFighter() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadFireFighter();
    });
}

void GameTextures::loadWatchman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadWatchman();
    });
}

void GameTextures::loadGoatherd() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGoatherd();
    });
}

void GameTextures::loadBronzeMiner() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadBronzeMiner();
    });
}

void GameTextures::loadOrichalcMiner() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOrichalcMiner();
    });
}

void GameTextures::loadArtisan() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadArtisan();
    });
}

void GameTextures::loadFoodVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadFoodVendor();
    });
}

void GameTextures::loadFleeceVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadFleeceVendor();
    });
}

void GameTextures::loadOilVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOilVendor();
    });
}

void GameTextures::loadWineVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadWineVendor();
    });
}

void GameTextures::loadArmsVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadArmsVendor();
    });
}

void GameTextures::loadHorseVendor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHorseVendor();
    });
}

void GameTextures::loadSheep() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSheep();
    });
}

void GameTextures::loadHorse() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHorse();
    });
}

void GameTextures::loadShepherd() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadShepherd();
    });
}

void GameTextures::loadMarbleMiner() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMarbleMiner();
    });
}

void GameTextures::loadSilverMiner() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSilverMiner();
    });
}

void GameTextures::loadArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadArcher();
    });
}

void GameTextures::loadPoseidonTowerArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPoseidonTowerArcher();
    });
}

void GameTextures::loadLumberjack() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadLumberjack();
    });
}

void GameTextures::loadTaxCollector() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTaxCollector();
    });
}

void GameTextures::loadTransporter() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTransporter();
    });
}

void GameTextures::loadGrower() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGrower();
    });
}

void GameTextures::loadTrader() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrader();
    });
}

void GameTextures::loadWaterDistributor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadWaterDistributor();
    });
}

void GameTextures::loadRockThrower() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadRockThrower();
    });
}

void GameTextures::loadHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHoplite();
    });
}

void GameTextures::loadHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHorseman();
    });
}

void GameTextures::loadHealer() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHealer();
    });
}

void GameTextures::loadCart() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCart();
    });
}

void GameTextures::loadBoar() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadBoar();
    });
}

void GameTextures::loadGymnast() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGymnast();
    });
}

void GameTextures::loadCompetitor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCompetitor();
    });
}

void GameTextures::loadGoat() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGoat();
    });
}

void GameTextures::loadWolf() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadWolf();
    });
}

void GameTextures::loadHunter() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHunter();
    });
}

void GameTextures::loadPhilosopher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPhilosopher();
    });
}

void GameTextures::loadUrchinGatherer() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadUrchinGatherer();
    });
}

void GameTextures::loadFishingBoat() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadFishingBoat();
    });
}

void GameTextures::loadTradeBoat() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTradeBoat();
    });
}

void GameTextures::loadTrireme() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrireme();
        c.loadTriremeOverlay();
    });
}

void GameTextures::loadEnemyBoat() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEnemyBoat();
    });
}

void GameTextures::loadDeer() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadDeer();
    });
}

void GameTextures::loadGreekHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGreekHoplite();
    });
}

void GameTextures::loadGreekHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGreekHorseman();
    });
}

void GameTextures::loadGreekRockThrower() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadGreekRockThrower();
    });
}

void GameTextures::loadTrojanHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrojanHoplite();
    });
}

void GameTextures::loadTrojanHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrojanHorseman();
    });
}

void GameTextures::loadTrojanSpearthrower() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTrojanSpearthrower();
    });
}

void GameTextures::loadCentaurHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCentaurHorseman();
    });
}

void GameTextures::loadCentaurArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCentaurArcher();
    });
}

void GameTextures::loadEgyptianHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEgyptianHoplite();
    });
}

void GameTextures::loadEgyptianChariot() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEgyptianChariot();
    });
}

void GameTextures::loadEgyptianArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEgyptianArcher();
    });
}

void GameTextures::loadMayanHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMayanHoplite();
    });
}

void GameTextures::loadMayanArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMayanArcher();
    });
}

void GameTextures::loadPhoenicianHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPhoenicianHorseman();
    });
}

void GameTextures::loadPhoenicianArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPhoenicianArcher();
    });
}

void GameTextures::loadOceanidHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOceanidHoplite();
    });
}

void GameTextures::loadOceanidSpearthrower() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOceanidSpearthrower();
    });
}

void GameTextures::loadAtlanteanHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAtlanteanHoplite();
    });
}

void GameTextures::loadAtlanteanChariot() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAtlanteanChariot();
    });
}

void GameTextures::loadAtlanteanArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAtlanteanArcher();
    });
}

void GameTextures::loadPersianHoplite() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPersianHoplite();
    });
}

void GameTextures::loadPersianHorseman() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPersianHorseman();
    });
}

void GameTextures::loadPersianArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPersianArcher();
    });
}

void GameTextures::loadAmazonSpear() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAmazonSpear();
    });
}

void GameTextures::loadAmazonArcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAmazonArcher();
    });
}

void GameTextures::loadAresWarrior() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAresWarrior();
    });
}

void GameTextures::loadDonkey() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadDonkey();
    });
}

void GameTextures::loadDisgruntled() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadDisgruntled();
    });
}

void GameTextures::loadSick() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSick();
    });
}

void GameTextures::loadRacingHorses() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadRacingHorses();
    });
}

void GameTextures::loadBanners() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadBanners();
    });
}


void GameTextures::loadCalydonianBoar() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCalydonianBoar();
    });
}

void GameTextures::loadCerberus() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCerberus();
    });
}

void GameTextures::loadChimera() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadChimera();
    });
}

void GameTextures::loadCyclops() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCyclops();
    });
}

void GameTextures::loadDragon() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadDragon();
    });
}

void GameTextures::loadEchidna() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEchidna();
    });
}

void GameTextures::loadHarpie() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHarpie();
    });
}

void GameTextures::loadHector() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHector();
    });
}

void GameTextures::loadHydra() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHydra();
    });
}

void GameTextures::loadKraken() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadKraken();
    });
}

void GameTextures::loadMaenads() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMaenads();
    });
}

void GameTextures::loadMedusa() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMedusa();
    });
}

void GameTextures::loadMinotaur() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadMinotaur();
    });
}

void GameTextures::loadScylla() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadScylla();
    });
}

void GameTextures::loadSphinx() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSphinx();
    });
}

void GameTextures::loadTalos() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTalos();
    });
}

void GameTextures::loadSatyr() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadSatyr();
    });
}

void GameTextures::loadAchilles() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAchilles();
    });
}

void GameTextures::loadAtalanta() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAtalanta();
    });
}

void GameTextures::loadBellerophon() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadBellerophon();
    });
}

void GameTextures::loadHeracles() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHeracles();
    });
}

void GameTextures::loadJason() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadJason();
    });
}

void GameTextures::loadOdysseus() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadOdysseus();
    });
}

void GameTextures::loadPerseus() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadPerseus();
    });
}

void GameTextures::loadTheseus() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadTheseus();
    });
}

void GameTextures::loadAphrodite() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadAphrodite();
    });
}

void GameTextures::loadApollo() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadApollo();
    });
}

void GameTextures::loadAres() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadAres();
    });
}

void GameTextures::loadArtemis() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadArtemis();
    });
}

void GameTextures::loadAthena() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadAthena();
    });
}

void GameTextures::loadAtlas() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadAtlas();
    });
}

void GameTextures::loadDemeter() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadDemeter();
    });
}

void GameTextures::loadDionysus() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadDionysus();
    });
}

void GameTextures::loadHades() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadHades();
    });
}

void GameTextures::loadHephaestus() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadHephaestus();
    });
}

void GameTextures::loadHera() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadHera();
    });
}

void GameTextures::loadHermes() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadHermes();
    });
}

void GameTextures::loadPoseidon() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadPoseidon();
    });
}

void GameTextures::loadZeus() {
    loadTexture([](const int i) {
        auto& c = sGodTextures[i];
        c.loadZeus();
    });
}

void GameTextures::loadFire() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadFire();
    });
}

void GameTextures::loadBlessed() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadBlessed();
    });
}

void GameTextures::loadCursed() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadCursed();
    });
}

void GameTextures::loadRock() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadRock();
    });
}

void GameTextures::loadBlackSpear() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadBlackSpear();
    });
}

void GameTextures::loadSpear() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadSpear();
    });
}

void GameTextures::loadBlackArrow() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadBlackArrow();
    });
}

void GameTextures::loadArrow() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadArrow();
    });
}

void GameTextures::loadBless() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadBless();
    });
}

void GameTextures::loadCurse() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadCurse();
    });
}

void GameTextures::loadPlague() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadPlague();
    });
}

void GameTextures::loadGodOrangeMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodOrangeMissile();
    });
}

void GameTextures::loadGodBlueArrow() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodBlueArrow();
    });
}

void GameTextures::loadGodOrangeArrow() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodOrangeArrow();
    });
}

void GameTextures::loadMonsterMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadMonsterMissile();
    });
}

void GameTextures::loadGodBlueMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodBlueMissile();
    });
}

void GameTextures::loadGodRedMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodRedMissile();
    });
}

void GameTextures::loadGodGreenMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodGreenMissile();
    });
}

void GameTextures::loadGodPinkMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodPinkMissile();
    });
}

void GameTextures::loadGodPurpleMissile() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadGodPurpleMissile();
    });
}

void GameTextures::loadWave() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadWave();
    });
}

void GameTextures::loadLava() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadLava();
    });
}

void GameTextures::loadDust() {
    loadTexture([](const int i) {
        auto& c = sDestructionTextures[i];
        c.loadDust();
    });
}

void GameTextures::loadCommonHouse() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadCommonHouse();
    });
}

void GameTextures::loadPoseidonCommonHouse() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPoseidonCommonHouse();
    });
}

void GameTextures::loadEliteHouse() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadEliteHouse();
    });
}

void GameTextures::loadPoseidonEliteHouse() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPoseidonEliteHouse();
    });
}

void GameTextures::loadCollege() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadCollege();
    });
}

void GameTextures::loadGymnasium() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadGymnasium();
    });
}

void GameTextures::loadDramaSchool() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadDramaSchool();
    });
}

void GameTextures::loadPodium() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPodium();
    });
}

void GameTextures::loadTheater() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTheater();
    });
}

void GameTextures::loadStadium() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadStadium();
    });
}

void GameTextures::loadFountain() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadFountain();
    });
}

void GameTextures::loadHospital() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHospital();
    });
}

void GameTextures::loadOliveTree() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadOliveTree();
    });
}

void GameTextures::loadVine() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadVine();
    });
}

void GameTextures::loadPlantation() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPlantation();
    });
}

void GameTextures::loadHuntingLodge() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHuntingLodge();
    });
}

void GameTextures::loadFishery() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadFishery();
    });
}

void GameTextures::loadUrchinQuay() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadUrchinQuay();
    });
}

void GameTextures::loadCardingShed() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadCardingShed();
    });
}

void GameTextures::loadDairy() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadDairy();
    });
}

void GameTextures::loadGrowersLodge() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadGrowersLodge();
    });
}

void GameTextures::loadTimberMill() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTimberMill();
    });
}

void GameTextures::loadMasonryShop() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadMasonryShop();
    });
}

void GameTextures::loadMint() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadMint();
    });
}

void GameTextures::loadFoundry() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadFoundry();
    });
}

void GameTextures::loadRefinery() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadRefinery();
    });
}

void GameTextures::loadArtisansGuild() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadArtisansGuild();
    });
}

void GameTextures::loadOlivePress() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadOlivePress();
    });
}

void GameTextures::loadWinery() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWinery();
    });
}

void GameTextures::loadSculptureStudio() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadSculptureStudio();
    });
}

void GameTextures::loadTriremeWharf() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTriremeWharf();
    });
}

void GameTextures::loadHorseRanch() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHorseRanch();
    });
}

void GameTextures::loadArmory() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadArmory();
    });
}

void GameTextures::loadGatehouseAndTower() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadGatehouseAndTower();
    });
}

void GameTextures::loadWall() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWall();
    });
}

void GameTextures::loadMaintenanceOffice() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadMaintenanceOffice();
    });
}

void GameTextures::loadTaxOffice() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTaxOffice();
    });
}

void GameTextures::loadWatchpost() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWatchpost();
    });
}

void GameTextures::loadRoadblock() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadRoadblock();
    });
}

void GameTextures::loadBridge() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadBridge();
    });
}

void GameTextures::loadPalace() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPalace();
    });
}

void GameTextures::loadPalaceTiles() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPalaceTiles();
    });
}

void GameTextures::loadSanctuary() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadSanctuary();
    });
}

void GameTextures::loadAltarBullOverlay() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAltarBullOverlay();
    });
}

void GameTextures::loadAltarSheepOverlay() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAltarSheepOverlay();
    });
}

void GameTextures::loadAltarGoodsOverlay() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAltarGoodsOverlay();
    });
}

void GameTextures::loadOrangeTendersLodge() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadOrangeTendersLodge();
    });
}

void GameTextures::loadWaterPark() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWaterPark();
    });
}

void GameTextures::loadShortObelisk() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadShortObelisk();
    });
}

void GameTextures::loadBirdBath() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadBirdBath();
    });
}

void GameTextures::loadBaths() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadBaths();
    });
}

void GameTextures::loadShellGarden() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadShellGarden();
    });
}

void GameTextures::loadOrrery() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadOrrery();
    });
}

void GameTextures::loadDolphinSculpture() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadDolphinSculpture();
    });
}

void GameTextures::loadStoneCircle() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadStoneCircle();
    });
}

void GameTextures::loadSpring() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadSpring();
    });
}

void GameTextures::loadTopiary() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTopiary();
    });
}

void GameTextures::loadSundial() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadSundial();
    });
}

void GameTextures::loadTallObelisk() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTallObelisk();
    });
}

void GameTextures::loadAvenue() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAvenue();
    });
}

void GameTextures::loadColumns() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadColumns();
    });
}

void GameTextures::loadCommemorative() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadCommemorative();
    });
}

void GameTextures::loadFishPond() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadFishPond();
    });
}

void GameTextures::loadHedgeMaze() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHedgeMaze();
    });
}

void GameTextures::loadGazebo() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadGazebo();
    });
}

void GameTextures::loadFlowerGarden() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadFlowerGarden();
    });
}

void GameTextures::loadBench() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadBench();
    });
}

void GameTextures::loadPark() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPark();
    });
}

void GameTextures::loadHippodrome() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHippodrome();
    });
}

void GameTextures::loadHippodromeSpectators() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHippodromeSpectators();
    });
}

void GameTextures::loadHippodromeFeces() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHippodromeFeces();
    });
}

void GameTextures::loadHippodromeFinish() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHippodromeFinish();
    });
}

void GameTextures::loadPyramid() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPyramid();
        c.loadPyramid2();
    });
}

void GameTextures::loadZeusMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadZeusMonuments();
    });
}

void GameTextures::loadPoseidonMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPoseidonMonuments();
    });
}

void GameTextures::loadHadesMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHadesMonuments();
    });
}

void GameTextures::loadDemeterMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadDemeterMonuments();
    });
}

void GameTextures::loadAthenaMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAthenaMonuments();
    });
}

void GameTextures::loadArtemisMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadArtemisMonuments();
    });
}

void GameTextures::loadApolloMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadApolloMonuments();
    });
}

void GameTextures::loadAresMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAresMonuments();
    });
}

void GameTextures::loadHephaestusMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHephaestusMonuments();
    });
}

void GameTextures::loadAphroditeMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAphroditeMonuments();
    });
}

void GameTextures::loadHermesMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHermesMonuments();
    });
}

void GameTextures::loadDionysusMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadDionysusMonuments();
    });
}

void GameTextures::loadHeraMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHeraMonuments();
    });
}

void GameTextures::loadAtlasMonuments() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAtlasMonuments();
    });
}

void GameTextures::loadHerosHall() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadHerosHall();
    });
}

void GameTextures::loadWaitingOverlay() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWaitingOverlay();
    });
}

void GameTextures::loadAgora() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadAgora();
    });
}

void GameTextures::loadPier() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPier();
    });
}

void GameTextures::loadTradingPost() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadTradingPost();
    });
}

void GameTextures::loadGranary() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadGranary();
    });
}

void GameTextures::loadMapOfGreece1() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece1();
    });
}

void GameTextures::loadMapOfGreece2() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece2();
    });
}

void GameTextures::loadMapOfGreece3() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece3();
    });
}

void GameTextures::loadMapOfGreece4() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece4();
    });
}

void GameTextures::loadMapOfGreece5() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece5();
    });
}

void GameTextures::loadMapOfGreece6() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece6();
    });
}

void GameTextures::loadMapOfGreece7() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece7();
    });
}

void GameTextures::loadMapOfGreece8() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadMapOfGreece8();
    });
}

void GameTextures::loadPoseidonMap1() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonMap1();
    });
}

void GameTextures::loadPoseidonMap2() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonMap2();
    });
}

void GameTextures::loadPoseidonMap3() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonMap3();
    });
}

void GameTextures::loadPoseidonMap4() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonMap4();
    });
}

void GameTextures::loadPoseidonCampaign1() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign1();
    });
}

void GameTextures::loadPoseidonCampaign2() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign2();
    });
}

void GameTextures::loadPoseidonCampaign3() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign3();
    });
}

void GameTextures::loadPoseidonCampaign4() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign4();
    });
}

void GameTextures::loadPoseidonCampaign5() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign5();
    });
}

void GameTextures::loadPoseidonCampaign6() {
    loadInterfaceTexture([](const int i) {
        auto& c = sInterfaceTextures[i];
        c.loadPoseidonCampaign6();
    });
}

void GameTextures::loadScholar() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadScholar();
    });
}

void GameTextures::loadAstronomer() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadAstronomer();
    });
}

void GameTextures::loadInventor() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadInventor();
    });
}

void GameTextures::loadCurator() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCurator();
    });
}

void GameTextures::loadBibliotheke() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadBibliotheke();
    });
}

void GameTextures::loadObservatory() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadObservatory();
    });
}

void GameTextures::loadUniversity() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadUniversity();
    });
}

void GameTextures::loadLaboratory() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadLaboratory();
    });
}

void GameTextures::loadInventorsWorkshop() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadInventorsWorkshop();
    });
}

void GameTextures::loadMuseum() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadMuseum();
    });
}

void GameTextures::loadChariotFactory() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadChariotFactory();
    });
}

void GameTextures::loadChariotVendor() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadChariotVendor();
    });
}

void GameTextures::loadChariotVendorCharacter() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadChariotVendor();
    });
}

void GameTextures::loadChariot() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadChariot();
    });
}

void GameTextures::loadElephant() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadElephant();
    });
}

void GameTextures::loadZeusSanctuary() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadZeusSanctuary();
    });
}

void GameTextures::loadPoseidonSanctuary() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPoseidonSanctuary();
    });
}

void GameTextures::loadPoseidonHerosHall() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadPoseidonHerosHall();
    });
}

void GameTextures::loadWineVendorBuilding() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadWineVendor();
    });
}

void GameTextures::loadHoplitePoseidon() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadHoplitePoseidon();
    });
}

void GameTextures::loadArcherPoseidon() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadArcherPoseidon();
    });
}

void GameTextures::loadChariotPoseidon() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadChariotPoseidon();
    });
}

void GameTextures::loadCorral() {
    loadTexture([](const int i) {
        auto& c = sBuildingTextures[i];
        c.loadCorral();
    });
}

void GameTextures::loadButcher() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadButcher();
    });
}

void GameTextures::loadCattle() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadCattle();
    });
}

void GameTextures::loadBull() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadBull();
    });
}

void GameTextures::loadPoseidonTrees() {
    loadTexture([](const int i) {
        auto& c = sTerrainTextures[i];
        c.loadPoseidonTrees();
    });
}

void GameTextures::loadOrichalc() {
    loadTexture([](const int i) {
        auto& c = sTerrainTextures[i];
        c.loadOrichalc();
    });
}

void GameTextures::loadEliteCitizen() {
    loadTexture([](const int i) {
        auto& c = sCharacterTextures[i];
        c.loadEliteCitizen();
    });
}

struct eFile {
    std::string fFilename;
    int fSize;
};

bool checkTextureFiles() {
    const std::vector<eFile> files = {
        {"zeus_hydra.555", 6793032},
        {"Poseidon_AtlasStat.555", 136293},
        {"Zeus_God02.sg3", 695080},
        {"Zeus_Sanctuary3.555", 367257},
        {"Zeus_MapOfGreece09.JPG", 255670},
        {"Zeus_persian.sg3", 991080},
        {"Zeus_Defeat.555", 1572864},
//            {"Zeus_DEMO1.555", 614400},
        {"zeus_statcon1.sg3", 695080},
        {"Poseidon_Chimera.555", 10049365},
//            {"Zeus_DEMO3.555", 614400},
        {"PoseidonImps2.sg3", 991080},
        {"zeus_talos.555", 3693451},
        {"zeus_statcon3.sg3", 695080},
        {"zeus_cerberus.555", 4694886},
        {"zeus_dragon.555", 6630383},
//            {"scoreb.jpg", 163708},
        {"Poseidon_Atlantean.555", 1229999},
        {"zeus_statcon6.555", 65680},
        {"Zeus_God05.sg3", 695080},
        {"zeus_satyr.555", 2092414},
        {"Poseidon_Egyptian.555", 1119386},
        {"Zeus_trojan.555", 881763},
        {"Zeus_AresWarriors.sg3", 991080},
        {"Zeus_MapOfGreece01.JPG", 271431},
        {"Poseidon_Atalanta.sg3", 991080},
        {"zeus_achilles.555", 1824423},
        {"Poseidon_Loaded.555", 9390226},
        {"Zeus_AresWarriors.555", 488655},
        {"zeus_perseus.sg3", 991080},
        {"Zeus_God02.555", 141890},
        {"PoseidonImps.sg3", 991080},
        {"Zeus_Load4.jpg", 160304},
        {"Zeus_MapOfGreece08.JPG", 255670},
        {"Zeus_God01.sg3", 695080},
        {"Poseidon_Sanctuary3.sg3", 695080},
        {"zeus_scylla.sg3", 991080},
        {"Poseidon_Unloaded.sg3", 695080},
        {"Poseidon_Egyptian.sg3", 991080},
        {"Zeus_God10.sg3", 695080},
        {"Poseidon_GrandStatue3.sg3", 695080},
        {"Zeus_Interface.sg3", 695080},
        {"Zeus_trojan.sg3", 991080},
        {"Zeus_Hades.sg3", 991080},
        {"Poseidon_Atlas.sg3", 991080},
        {"Zeus_Hermes.555", 4170959},
        {"zeus_maenads.sg3", 991080},
        {"Poseidon_Phoenician.555", 565558},
        {"zeus_heracles.555", 1478018},
        {"Poseidon_Oceanid.555", 352558},
//            {"Zeus_FE_ChooseGame.jpg", 273777},
        {"Zeus_Zeus.sg3", 991080},
        {"Poseidon_map02.jpg", 519337},
        {"zeus_hector.sg3", 991080},
        {"Zeus_Greek.555", 1864773},
        {"Zeus_MapOfGreece04.JPG", 276583},
        {"Zeus_Sanctuary2.sg3", 695080},
        {"Zeus_MapOfGreece07.JPG", 272842},
        {"zeus_maenads.555", 3473119},
        {"Zeus_Athena.sg3", 991080},
        {"Zeus_Load3.jpg", 136115},
        {"Poseidon_Harpie.sg3", 991080},
        {"Zeus_WorldThumbnails.555", 1013600},
        {"Poseidon_GrandStatue3.555", 517508},
        {"Zeus_Zeus.555", 9436703},
        {"zeus_hydra.sg3", 991080},
        {"Zeus_centaur.555", 861854},
        {"Poseidon_Bellerophon.sg3", 991080},
        {"zeus_odysseus.sg3", 991080},
        {"Zeus_MapOfGreece01.sg3", 695080},
        {"Poseidon_Sphinx.sg3", 991080},
        {"Zeus_Victory.jpg", 254798},
        {"Zeus_General.555", 6195784},
        {"Poseidon_GrandStatue1.555", 458157},
//            {"Zeus_Title.jpg", 134497},
        {"Zeus_God03.sg3", 695080},
        {"Zeus_Sanctuary3.sg3", 695080},
        {"zeus_statcon4.sg3", 695080},
        {"Zeus_General.sg3", 695080},
        {"destruction.sg3", 991080},
        {"PoseidonImps2.555", 5845803},
        {"zeus_odysseus.555", 3632645},
//            {"Zeus_FE_MainMenu.jpg", 273122},
        {"Poseidon_Sanctuary3.555", 367257},
        {"Poseidon_Loaded.sg3", 695080},
        {"zeus_jason.sg3", 991080},
        {"Zeus_Ares.sg3", 991080},
        {"zeus_kraken.555", 11185029},
        {"Zeus_God07.sg3", 695080},
        {"Poseidon_Oceanid.sg3", 991080},
        {"Zeus_Sanctuary1.555", 189914},
        {"PoseidonImps.555", 25251109},
        {"Zeus_MapOfGreece06.JPG", 277229},
        {"zeus_hector.555", 2743516},
//            {"Poseidon_FE_MainMenu.JPG", 176221},
        {"Poseidon_Echidna.555", 7579780},
        {"Zeus_Artemis.sg3", 991080},
        {"zeus_kraken.sg3", 991080},
        {"Poseidon_Load1.jpg", 149618},
        {"Poseidon_Load3.jpg", 109507},
        {"Poseidon_GrandStatue2.555", 520078},
        {"Poseidon_Load7.jpg", 230960},
        {"Poseidon_Phoenician.sg3", 991080},
        {"Poseidon_Bellerophon.555", 4518763},
        {"Zeus_MapOfGreece10.JPG", 255670},
        {"zeus_statcon6.sg3", 695080},
        {"Poseidon_Atlas.555", 8089871},
        {"Zeus_Poseidon.555", 5330976},
        {"Zeus_Hermes.sg3", 991080},
        {"Zeus_Sanctuary1.sg3", 695080},
        {"Poseidon_Portraits.555", 391050},
        {"Zeus_Demeter.555", 4801528},
        {"Zeus_Aphrodite.sg3", 991080},
        {"Zeus_Load2.jpg", 168342},
        {"Zeus_Dionysus.sg3", 991080},
//            {"Poseidon_FE_Registry.jpg", 217089},
        {"Poseidon_map01.jpg", 482447},
//            {"Tutorial2.555", 380110},
        {"Poseidon_Load8.jpg", 210231},
        {"Zeus_God12.sg3", 695080},
        {"Zeus_God01.555", 139549},
        {"Poseidon_Load6.jpg", 223208},
        {"zeus_statcon1.555", 84839},
        {"Zeus_God07.555", 137345},
        {"Poseidon_GrandStatue1.sg3", 695080},
        {"zeus_theseus.sg3", 991080},
        {"Zeus_God10.555", 137678},
        {"zeus_calydonianboar.555", 2050608},
        {"Zeus_Poseidon.sg3", 991080},
        {"Zeus_God11.sg3", 695080},
        {"Poseidon_Load2.jpg", 205428},
        {"Zeus_God09.sg3", 695080},
        {"zeus_cerberus.sg3", 991080},
        {"Zeus_MapOfGreece03.JPG", 268073},
        {"Poseidon_Atalanta.555", 2200056},
        {"Zeus_Hephaestus.555", 2375563},
        {"Poseidon_Hera.555", 5931730},
        {"Poseidon_WorldThumbnails.555", 152040},
        {"zeus_perseus.555", 1556654},
        {"Poseidon_Load5.jpg", 199072},
        {"zeus_statcon5.555", 65680},
        {"Zeus_Load1.jpg", 195191},
        {"Poseidon_map04.jpg", 588190},
        {"zeus_achilles.sg3", 991080},
        {"SprMain.sg3", 991080},
        {"zeus_Unloaded.555", 39218},
        {"God_Portraits.555", 278080},
        {"Poseidon_map03.jpg", 466302},
        {"destruction.555", 5725044},
        {"Zeus_Apollo.555", 5604649},
        {"zeus_cyclops.sg3", 991080},
        {"SprMain.555", 15270017},
        {"Zeus_Apollo.sg3", 991080},
        {"zeus_statcon3.555", 65680},
//            {"Picture9.555", 964040},
        {"zeus_calydonianboar.sg3", 991080},
        {"Zeus_amazon.555", 618167},
        {"Poseidon_Chimera.sg3", 991080},
//        {"Zeus_Fonts.sg3", 695080},
        {"zeus_minotaur.555", 4964285},
        {"Zeus_God06.sg3", 695080},
        {"Poseidon_HeraStat.sg3", 695080},
        {"zeus_statcon2.sg3", 695080},
        {"zeus_medusa.sg3", 991080},
//            {"Zeus_DEMO2.555", 614400},
//            {"map_panels.555", 248832},
        {"zeus_dragon.sg3", 991080},
        {"Poseidon_Harpie.555", 8167597},
        {"zeus_cyclops.555", 4779943},
        {"zeus_statcon5.sg3", 695080},
//            {"Tutorial1.555", 593220},
        {"Zeus_Demeter.sg3", 991080},
        {"Poseidon_Sphinx.555", 8250702},
        {"zeus_scylla.555", 9469592},
        {"Zeus_Terrain.555", 4210543},
        {"Zeus_God09.555", 131916},
        {"Zeus_Interface.555", 4461887},
        {"Poseidon_Hera.sg3", 991080},
//        {"zeus_Fonts.555", 489709},
        {"SprAmbient.sg3", 991080},
        {"Poseidon_Echidna.sg3", 991080},
        {"Zeus_centaur.sg3", 991080},
        {"CampaignIllustrations.555", 2160000},
        {"Poseidon_GrandStatue2.sg3", 695080},
        {"Zeus_God08.sg3", 695080},
        {"Poseidon_Mayan.sg3", 991080},
        {"Zeus_God03.555", 131516},
        {"zeus_talos.sg3", 991080},
        {"Zeus_FE_Registry.jpg", 154974},
        {"Zeus_Defeat.jpg", 196124},
//            {"Zeus_FE_tutorials.JPG", 265818},
        {"Zeus_amazon.sg3", 991080},
        {"Zeus_Greek.sg3", 991080},
        {"Zeus_Aphrodite.555", 3627738},
        {"Portraits.555", 1051490},
        {"Zeus_Dionysus.555", 6361139},
        {"Zeus_God11.555", 122320},
        {"zeus_statcon2.555", 65680},
        {"Zeus_Ares.555", 5374332},
        {"Zeus_MapOfGreece02.JPG", 274035},
        {"Zeus_persian.555", 1103880},
        {"Poseidon_HeraStat.555", 134186},
        {"Zeus_FE_MissionIntroduction.jpg", 152920},
        {"Poseidon_Mayan.555", 489119},
        {"Zeus_Hades.555", 4021368},
        {"zeus_satyr.sg3", 991080},
        {"zeus_statcon4.555", 65680},
        {"Zeus_God05.555", 142461},
        {"Poseidon_AtlasStat.sg3", 695080},
        {"Zeus_Athena.555", 3467534},
        {"zeus_minotaur.sg3", 991080},
        {"Zeus_Terrain.sg3", 695080},
        {"Zeus_God08.555", 138697},
        {"zeus_theseus.555", 2034580},
        {"Zeus_God06.555", 127529},
        {"zeus_jason.555", 2541355},
//            {"Zeus_FE_CampaignSelection.JPG", 190012},
        {"Zeus_God12.555", 159323},
        {"Poseidon_Atlantean.sg3", 991080},
        {"Poseidon_Unloaded.555", 89898},
        {"zeus_medusa.555", 3576724},
        {"zeus_heracles.sg3", 991080},
        {"Poseidon_Load4.jpg", 340306},
        {"Zeus_God04.555", 131644},
        {"Zeus_MapOfGreece05.JPG", 275769},
        {"Zeus_God04.sg3", 695080},
        {"Zeus_Hephaestus.sg3", 991080},
        {"Zeus_Unloaded.sg3", 695080},
        {"Zeus_Sanctuary2.555", 307672},
        {"Zeus_Artemis.555", 3198753},
        {"SprAmbient.555", 5654853},
    };

    bool missing = false;
    for(const auto& f : files) {
        const auto path = GameDir::path("DATA/" + f.fFilename);
        const bool e = std::filesystem::exists(path);
        if(!e) {
            missing = true;
            printf("File missing '%s'\n", path.c_str());
        }
    }

    return !missing;
}

bool GameTextures::initialize(SDL_Renderer* const r) {
    if(sInitialized) return true;
//    const bool e = checkTextureFiles();
    const auto path = GameDir::path("DATA");
    const bool e = std::filesystem::exists(path);
    if(!e) {
        printf("DATA folder missing from Zeus and Poseidon directory.\n"
               "Expected to find %s\n",
               path.c_str());
        return false;
    }
    int i = 0;
    for(const auto& s : {std::pair<int, int>{30, 15},
                         std::pair<int, int>{60, 30},
                         std::pair<int, int>{90, 45},
                         std::pair<int, int>{120, 60}}) {
        sTerrainTextures.emplace_back(s.first, s.second, r);
        sGodTextures.emplace_back(s.first, s.second, r);
        sBuildingTextures.emplace_back(s.first, s.second, r);
        sCharacterTextures.emplace_back(s.first, s.second, r);
        sInterfaceTextures.emplace_back(s.first, s.second, r);
        sDestructionTextures.emplace_back(s.first, s.second, r);

        gGameLoaders.emplace_back([i](std::string& text) {
            sTerrainTextures[i].load();
            if(i == 0) {
                text = "Loading tiny terrain textures...";
            } else if(i == 1) {
                text = "Loading small terrain textures...";
            } else if(i == 2) {
                text = "Loading medium terrain textures...";
            } else if(i == 3) {
                text = "Loading large terrain textures...";
            }
        }, i);

        gGameLoaders.emplace_back([i](std::string& text) {
            sBuildingTextures[i].load();
            if(i == 0) {
                text = "Loading tiny building textures...";
            } else if(i == 1) {
                text = "Loading small building textures...";
            } else if(i == 2) {
                text = "Loading medium building textures...";
            } else if(i == 3) {
                text = "Loading large building textures...";
            }
        }, i);

        gMenuLoaders.emplace_back([i](std::string& text) {
            sInterfaceTextures[i].load();
            if(i == 0) {
                text = "Loading tiny interface textures...";
            } else if(i == 1) {
                text = "Loading small interface textures...";
            } else if(i == 2) {
                text = "Loading medium interface textures...";
            } else if(i == 3) {
                text = "Loading large interface textures...";
            }
        }, i);

        i++;
    }

    sInitialized = true;

    return true;
}

bool GameTextures::loadNextMenu(const Settings& settings,
                                 std::string& text) {
    const int iMax = gMenuLoaders.size();
    const auto uiScale = settings.fUiScale;
    for(int i = 0; i < iMax; i++) {
        auto& g = gMenuLoaders[i];
        if(g.fFinished) continue;
        if(uiScale != eUIScale::tiny &&
           g.fSize == 0) continue;
        if(uiScale != eUIScale::small &&
           g.fSize == 1) continue;
        if(uiScale != eUIScale::medium &&
           g.fSize == 2) continue;
        if(uiScale != eUIScale::large &&
           g.fSize == 3) continue;
        g.fFunc(text);
        g.fFinished = true;
        return false;
    }
    text = "Finished";
    return true;
}

bool GameTextures::loadNextGame(const Settings& settings,
                                 std::string& text) {
    const int iMax = gGameLoaders.size();
    for(int i = 0; i < iMax; i++) {
        auto& g = gGameLoaders[i];
        if(g.fFinished) continue;
        if(!settings.fTinyTextures &&
           g.fSize == 0) continue;
        if(!settings.fSmallTextures &&
           g.fSize == 1) continue;
        if(!settings.fMediumTextures &&
           g.fSize == 2) continue;
        if(!settings.fLargeTextures &&
           g.fSize == 3) continue;
        g.fFunc(text);
        g.fFinished = true;
        return false;
    }
    text = "Finished";
    return true;
}

int GameTextures::gameSize(const Settings& settings) {
    int result = 0;
    const int iMax = gGameLoaders.size();
    for(int i = 0; i < iMax; i++) {
        auto& g = gGameLoaders[i];
        if(g.fFinished) continue;
        if(!settings.fTinyTextures &&
           g.fSize == 0) continue;
        if(!settings.fSmallTextures &&
           g.fSize == 1) continue;
        if(!settings.fMediumTextures &&
           g.fSize == 2) continue;
        if(!settings.fLargeTextures &&
           g.fSize == 3) continue;
        result++;
    }
    return result;
}

int GameTextures::menuSize() {
    return sInterfaceTextures.size();
}

void GameTextures::setSettings(const Settings& s) {
    sSettings = s;
}
