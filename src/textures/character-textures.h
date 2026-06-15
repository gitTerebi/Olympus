#ifndef CHARACTER_TEXTURES_H
#define CHARACTER_TEXTURES_H

#include "texture-collection.h"

struct BasicCharacterTextures {
    BasicCharacterTextures(SDL_Renderer* const renderer) :
        fDie(renderer) {}

    std::vector<TextureCollection> fWalk;
    TextureCollection fDie;
};

struct FishingBoatTextures {
    FishingBoatTextures(SDL_Renderer* const renderer) :
        fStand(renderer) {}

    TextureCollection fStand;
    std::vector<TextureCollection> fSwim;
    std::vector<TextureCollection> fCollect;
    std::vector<TextureCollection> fDie;
};

struct UrchinGathererTextures {
    UrchinGathererTextures(SDL_Renderer* const renderer) :
        fDie(renderer) {}

    std::vector<TextureCollection> fSwim;
    std::vector<TextureCollection> fCollect;
    std::vector<TextureCollection> fCarry;
    std::vector<TextureCollection> fDeposit;
    TextureCollection fDie;
};

struct TradeBoatTextures {
    TradeBoatTextures(SDL_Renderer* const renderer) :
        fStand(renderer) {}

    TextureCollection fStand;
    std::vector<TextureCollection> fSwim;
    std::vector<TextureCollection> fDie;
};

struct ResourceCollectorTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fCarry;
    std::vector<TextureCollection> fCollect;
};

struct ArtisanTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fBuild;
    std::vector<TextureCollection> fBuildStanding;
};

struct ShepherdTextures : public BasicCharacterTextures {
    ShepherdTextures(SDL_Renderer* const renderer) :
        BasicCharacterTextures(renderer),
        fCollect(renderer),
        fFight(renderer) {}

    std::vector<TextureCollection> fCarry;
    TextureCollection fCollect;
    TextureCollection fFight;
};

struct OrangeTenderTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fWorkOnTree;
    std::vector<TextureCollection> fCollect;
};

struct GrowerTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fWorkOnGrapes;
    std::vector<TextureCollection> fWorkOnOlives;

    std::vector<TextureCollection> fCollectGrapes;
    std::vector<TextureCollection> fCollectOlives;
};

struct FightingCharacterTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fFight;
};

struct FireFighterTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fCarry;
    std::vector<TextureCollection> fPutOut;
};

struct AnimalTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fFight;
    std::vector<TextureCollection> fLayDown;
};

struct HorseTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fStand;
};

struct CattleTextures : public BasicCharacterTextures {
    using BasicCharacterTextures::BasicCharacterTextures;

    std::vector<TextureCollection> fStand;
};

struct BullTextures : public CattleTextures {
    using CattleTextures::CattleTextures;

    std::vector<TextureCollection> fAttack;
};

struct ArcherTextures : public FightingCharacterTextures {
    using FightingCharacterTextures::FightingCharacterTextures;

    std::vector<TextureCollection> fPatrol;
};

struct RockThrowerTextures : public FightingCharacterTextures {
    using FightingCharacterTextures::FightingCharacterTextures;

    std::vector<TextureCollection> fFight2;
};

struct MonsterTextures {
    std::vector<TextureCollection> fWalk;
    std::vector<TextureCollection> fDie;
    std::vector<TextureCollection> fFight;
    std::vector<TextureCollection> fFight2;
};

struct HeroTextures {
    std::vector<TextureCollection> fWalk;
    std::vector<TextureCollection> fDie;
    std::vector<TextureCollection> fFight;
};

struct AresWarriorTextures {
    std::vector<TextureCollection> fWalk;
    std::vector<TextureCollection> fDie;
    std::vector<TextureCollection> fFight;
};

struct ChariotTextures {
    std::vector<TextureCollection> fWalk;
    std::vector<TextureCollection> fDie;
    std::vector<TextureCollection> fFight;
};

struct WaterMonsterTextures {
    WaterMonsterTextures(SDL_Renderer* const renderer) :
        fDie(renderer) {}

    std::vector<TextureCollection> fWalk;
    TextureCollection fDie;
    std::vector<TextureCollection> fFight;
    std::vector<TextureCollection> fFight2;
};

struct RacingHorseTextures {
    RacingHorseTextures(SDL_Renderer* const renderer) :
        fDie(renderer) {}

    std::vector<TextureCollection> fRace;
    std::vector<TextureCollection> fStand;
    TextureCollection fDie;
};

class CharacterTextures {
public:
    CharacterTextures(const int tileW, const int tileH,
                       SDL_Renderer* const renderer);

    void loadAll();

    bool fPeddlerLoaded = false;
    void loadPeddler();
    bool fActorLoaded = false;
    void loadActor();
    bool fOxLoaded = false;
    void loadOx();
    bool fPorterLoaded = false;
    void loadPorter();
    bool fOxHandlerLoaded = false;
    void loadOxHandler();
    bool fTrailerLoaded = false;
    void loadTrailer();
    bool fSettlersLoaded = false;
    void loadSettlers();
    bool fFireFighterLoaded = false;
    void loadFireFighter();
    bool fWatchmanLoaded = false;
    void loadWatchman();
    bool fGoatherdLoaded = false;
    void loadGoatherd();
    bool fBronzeMinerLoaded = false;
    void loadBronzeMiner();
    bool fOrichalcMinerLoaded = false;
    void loadOrichalcMiner();
    bool fArtisanLoaded = false;
    void loadArtisan();
    bool fFoodVendorLoaded = false;
    void loadFoodVendor();
    bool fFleeceVendorLoaded = false;
    void loadFleeceVendor();
    bool fOilVendorLoaded = false;
    void loadOilVendor();
    bool fWineVendorLoaded = false;
    void loadWineVendor();
    bool fArmsVendorLoaded = false;
    void loadArmsVendor();
    bool fHorseVendorLoaded = false;
    void loadHorseVendor();
    bool fSheepLoaded = false;
    void loadSheep();
    bool fHorseLoaded = false;
    void loadHorse();
    bool fShepherdLoaded = false;
    void loadShepherd();
    bool fMarbleMinerLoaded = false;
    void loadMarbleMiner();
    bool fSilverMinerLoaded = false;
    void loadSilverMiner();
    bool fArcherLoaded = false;
    void loadArcher();
    bool fPoseidonTowerArcherLoaded = false;
    void loadPoseidonTowerArcher();
    bool fLumberjackLoaded = false;
    void loadLumberjack();
    bool fTaxCollectorLoaded = false;
    void loadTaxCollector();
    bool fTransporterLoaded = false;
    void loadTransporter();
    bool fGrowerLoaded = false;
    void loadGrower();
    bool fOrangeTenderLoded = false;
    void loadOrangeTender();
    bool fTraderLoaded = false;
    void loadTrader();
    bool fWaterDistributorLoaded = false;
    void loadWaterDistributor();

    bool fRockThrowerLoaded = false;
    void loadRockThrower();
    bool fHopliteLoaded = false;
    void loadHoplite();
    bool fHorsemanLoaded = false;
    void loadHorseman();

    bool fAmazonSpearLoaded = false;
    void loadAmazonSpear();
    bool fAmazonArcherLoaded = false;
    void loadAmazonArcher();

    bool fTrojanHopliteLoaded = false;
    void loadTrojanHoplite();
    bool fTrojanSpearthrowerLoaded = false;
    void loadTrojanSpearthrower();
    bool fTrojanHorsemanLoaded = false;
    void loadTrojanHorseman();

    bool fCentaurHorsemanLoaded = false;
    void loadCentaurHorseman();
    bool fCentaurArcherLoaded = false;
    void loadCentaurArcher();

    bool fPersianHopliteLoaded = false;
    void loadPersianHoplite();
    bool fPersianHorsemanLoaded = false;
    void loadPersianHorseman();
    bool fPersianArcherLoaded = false;
    void loadPersianArcher();

    bool fEgyptianHopliteLoaded = false;
    void loadEgyptianHoplite();
    bool fEgyptianChariotLoaded = false;
    void loadEgyptianChariot();
    bool fEgyptianArcherLoaded = false;
    void loadEgyptianArcher();

    bool fMayanHopliteLoaded = false;
    void loadMayanHoplite();
    bool fMayanArcherLoaded = false;
    void loadMayanArcher();

    bool fOceanidHopliteLoaded = false;
    void loadOceanidHoplite();
    bool fOceanidSpearthrowerLoaded = false;
    void loadOceanidSpearthrower();

    bool fPhoenicianHorsemanLoaded = false;
    void loadPhoenicianHorseman();
    bool fPhoenicianArcherLoaded = false;
    void loadPhoenicianArcher();

    bool fAtlanteanHopliteLoaded = false;
    void loadAtlanteanHoplite();
    bool fAtlanteanChariotLoaded = false;
    void loadAtlanteanChariot();
    bool fAtlanteanArcherLoaded = false;
    void loadAtlanteanArcher();

    bool fAresWarriorLoaded = false;
    void loadAresWarrior();

    bool fHealerLoaded = false;
    void loadHealer();
    bool fCartLoaded = false;
    void loadCart();
    bool fOrichalcCartLoaded = false;
    void loadOrichalcCart();
    bool fOrangesCartLoaded = false;
    void loadOrangesCart();
    bool fBlackMarbleTrailerLoaded = false;
    void loadBlackMarbleTrailer();
    bool fBoarLoaded = false;
    void loadBoar();
    bool fGymnastLoaded = false;
    void loadGymnast();
    bool fCompetitorLoaded = false;
    void loadCompetitor();
    bool fGoatLoaded = false;
    void loadGoat();
    bool fWolfLoaded = false;
    void loadWolf();
    bool fHunterLoaded = false;
    void loadHunter();
    bool fDeerHunterLoaded = false;
    void loadDeerHunter();
    bool fPhilosopherLoaded = false;
    void loadPhilosopher();
    bool fUrchinGathererLoaded = false;
    void loadUrchinGatherer();
    bool fFishingBoatLoaded = false;
    void loadFishingBoat();
    bool fTradeBoatLoaded = false;
    void loadTradeBoat();
    bool fTriremeLoaded = false;
    void loadTrireme();
    bool fTriremeOverlayLoaded = false;
    void loadTriremeOverlay();
    bool fEnemyBoatLoaded = false;
    void loadEnemyBoat();
    bool fDeerLoaded = false;
    void loadDeer();
    bool fGreekHopliteLoaded = false;
    void loadGreekHoplite();
    bool fGreekHorsemanLoaded = false;
    void loadGreekHorseman();
    bool fGreekRockThrowerLoaded = false;
    void loadGreekRockThrower();
    bool fPriestLoaded = false;
    void loadPriest();

    bool fDonkeyLoaded = false;
    void loadDonkey();

    bool fDisgruntledLoaded = false;
    void loadDisgruntled();
    bool fSickLoaded = false;
    void loadSick();
    bool fHomelessLoaded = false;
    void loadHomeless();

    bool fCalydonianBoarLoaded = false;
    void loadCalydonianBoar();
    bool fCerberusLoaded = false;
    void loadCerberus();
    bool fChimeraLoaded = false;
    void loadChimera();
    bool fCyclopsLoaded = false;
    void loadCyclops();
    bool fDragonLoaded = false;
    void loadDragon();
    bool fEchidnaLoaded = false;
    void loadEchidna();
    bool fHarpieLoaded = false;
    void loadHarpie();
    bool fHectorLoaded = false;
    void loadHector();
    bool fHydraLoaded = false;
    void loadHydra();
    bool fKrakenLoaded = false;
    void loadKraken();
    bool fMaenadsLoaded = false;
    void loadMaenads();
    bool fMedusaLoaded = false;
    void loadMedusa();
    bool fMinotaurLoaded = false;
    void loadMinotaur();
    bool fScyllaLoaded = false;
    void loadScylla();
    bool fSphinxLoaded = false;
    void loadSphinx();
    bool fTalosLoaded = false;
    void loadTalos();

    bool fAchillesLoaded = false;
    void loadAchilles();
    bool fAtalantaLoaded = false;
    void loadAtalanta();
    bool fBellerophonLoaded = false;
    void loadBellerophon();
    bool fHeraclesLoaded = false;
    void loadHeracles();
    bool fJasonLoaded = false;
    void loadJason();
    bool fOdysseusLoaded = false;
    void loadOdysseus();
    bool fPerseusLoaded = false;
    void loadPerseus();
    bool fTheseusLoaded = false;
    void loadTheseus();

    bool fScholarLoaded = false;
    void loadScholar();
    bool fAstronomerLoaded = false;
    void loadAstronomer();
    bool fInventorLoaded = false;
    void loadInventor();
    bool fCuratorLoaded = false;
    void loadCurator();

    bool fHoplitePoseidonLoaded = false;
    void loadHoplitePoseidon();
    bool fArcherPoseidonLoaded = false;
    void loadArcherPoseidon();
    bool fChariotPoseidonLoaded = false;
    void loadChariotPoseidon();

    bool fCattleLoaded = false;
    void loadCattle();
    bool fBullLoaded = false;
    void loadBull();
    bool fButcherLoaded = false;
    void loadButcher();

    bool fChariotVendorLoaded = false;
    void loadChariotVendor();

    bool fChariotLoaded = false;
    void loadChariot();

    bool fElephantLoaded = false;
    void loadElephant();

    bool fSatyrLoaded = false;
    void loadSatyr();

    bool fBannersLoaded = false;
    void loadBanners();

    bool fEliteCitizenLoaded = false;
    void loadEliteCitizen();

    bool fRacingHorsesLoaded = false;
    void loadRacingHorses();

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    BasicCharacterTextures fPeddler;

    BasicCharacterTextures fActor;
    BasicCharacterTextures fTaxCollector;
    BasicCharacterTextures fWaterDistributor;
    FightingCharacterTextures fWatchman;
    FireFighterTextures fFireFighter;
    BasicCharacterTextures fHealer;
    BasicCharacterTextures fGymnast;
    BasicCharacterTextures fCompetitor;
    BasicCharacterTextures fPhilosopher;

    BasicCharacterTextures fOx;
    BasicCharacterTextures fOxHandler;

    TextureCollection fEmptyTrailer;
    TextureCollection fWoodTrailer1;
    TextureCollection fWoodTrailer2;
    TextureCollection fMarbleTrailer1;
    TextureCollection fMarbleTrailer2;
    TextureCollection fBlackMarbleTrailer1;
    TextureCollection fBlackMarbleTrailer2;
    TextureCollection fSculptureTrailer;

    TextureCollection fEmptyBigTrailer;
    TextureCollection fMarbleBigTrailer;
    TextureCollection fBlackMarbleBigTrailer;

    ResourceCollectorTextures fMarbleMiner;
    ResourceCollectorTextures fSilverMiner;
    ResourceCollectorTextures fBronzeMiner;
    ResourceCollectorTextures fOrichalcMiner;
    ResourceCollectorTextures fLumberjack;

    ArtisanTextures fArtisan;

    ResourceCollectorTextures fHunter;
    ResourceCollectorTextures fDeerHunter;

    ShepherdTextures fShepherd;
    ShepherdTextures fGoatherd;

    BasicCharacterTextures fFoodVendor;
    BasicCharacterTextures fFleeceVendor;
    BasicCharacterTextures fOilVendor;
    BasicCharacterTextures fWineVendor;
    BasicCharacterTextures fArmsVendor;
    BasicCharacterTextures fHorseVendor;

    GrowerTextures fGrower;

    AnimalTextures fBoar;
    AnimalTextures fDeer;
    AnimalTextures fWolf;

    CattleTextures fCattle1;
    CattleTextures fCattle2;
    CattleTextures fCattle3;
    BullTextures fBull;
    BasicCharacterTextures fButcher;

    AnimalTextures fGoat;
    AnimalTextures fNudeSheep;
    AnimalTextures fFleecedSheep;
    HorseTextures fHorse;

    BasicCharacterTextures fSettlers1;
    BasicCharacterTextures fSettlers2;

    BasicCharacterTextures fTransporter;

    TextureCollection fEmptyCart;
    std::vector<TextureCollection> fUrchinCart;
    std::vector<TextureCollection> fFishCart;
    std::vector<TextureCollection> fMeatCart;
    std::vector<TextureCollection> fCheeseCart;
    std::vector<TextureCollection> fCarrotsCart;
    std::vector<TextureCollection> fOnionsCart;
    std::vector<TextureCollection> fWheatCart;
    std::vector<TextureCollection> fBronzeCart;
    std::vector<TextureCollection> fGrapesCart;
    std::vector<TextureCollection> fOlivesCart;
    std::vector<TextureCollection> fFleeceCart;
    std::vector<TextureCollection> fArmorCart;
    std::vector<TextureCollection> fOliveOilCart;
    std::vector<TextureCollection> fWineCart;

    std::vector<TextureCollection> fOrangesCart;
    std::vector<TextureCollection> fOrichalcCart;

    OrangeTenderTextures fOrangeTender;

    ArcherTextures fArcher;
    ArcherTextures fPoseidonTowerArcher;

    RockThrowerTextures fRockThrower;
    FightingCharacterTextures fHoplite;
    FightingCharacterTextures fHorseman;

    FightingCharacterTextures fHoplitePoseidon;
    ChariotTextures fChariotPoseidon;
    FightingCharacterTextures fArcherPoseidon;

    RockThrowerTextures fGreekRockThrower;
    FightingCharacterTextures fGreekHoplite;
    FightingCharacterTextures fGreekHorseman;

    FightingCharacterTextures fAmazonSpear;
    FightingCharacterTextures fAmazonArcher;

    FightingCharacterTextures fTrojanHoplite;
    FightingCharacterTextures fTrojanSpearthrower;
    FightingCharacterTextures fTrojanHorseman;

    FightingCharacterTextures fCentaurHorseman;
    FightingCharacterTextures fCentaurArcher;

    FightingCharacterTextures fEgyptianHoplite;
    ChariotTextures fEgyptianChariot;
    FightingCharacterTextures fEgyptianArcher;

    FightingCharacterTextures fMayanHoplite;
    FightingCharacterTextures fMayanArcher;

    FightingCharacterTextures fPhoenicianHorseman;
    FightingCharacterTextures fPhoenicianArcher;

    FightingCharacterTextures fOceanidHoplite;
    FightingCharacterTextures fOceanidSpearthrower;

    FightingCharacterTextures fPersianHoplite;
    FightingCharacterTextures fPersianHorseman;
    FightingCharacterTextures fPersianArcher;

    FightingCharacterTextures fAtlanteanHoplite;
    ChariotTextures fAtlanteanChariot;
    FightingCharacterTextures fAtlanteanArcher;

    AresWarriorTextures fAresWarrior;

    TextureCollection fBannerRod;
    std::vector<TextureCollection> fBanners;
    TextureCollection fBannerTops;
    TextureCollection fPoseidonBannerTops;

    BasicCharacterTextures fPriest;

    BasicCharacterTextures fTrader;
    BasicCharacterTextures fDonkey;

    BasicCharacterTextures fPorter;

    FishingBoatTextures fFishingBoat;
    UrchinGathererTextures fUrchinGatherer;
    TradeBoatTextures fTradeBoat;
    TradeBoatTextures fTrireme;
    std::vector<TextureCollection> fTriremeOverlay;
    std::vector<TextureCollection> fTriremeFightOverlay;
    std::vector<TextureCollection> fTriremeDieOverlay;
    TradeBoatTextures fEnemyBoat;

    FightingCharacterTextures fDisgruntled;
    FightingCharacterTextures fSick;
    BasicCharacterTextures fHomeless;

    MonsterTextures fCalydonianBoar;
    MonsterTextures fCerberus;
    MonsterTextures fChimera;
    MonsterTextures fCyclops;
    MonsterTextures fDragon;
    MonsterTextures fEchidna;
    MonsterTextures fHarpies;
    MonsterTextures fHector;
    MonsterTextures fHydra;
    WaterMonsterTextures fKraken;
    MonsterTextures fMaenads;
    MonsterTextures fMedusa;
    MonsterTextures fMinotaur;
    WaterMonsterTextures fScylla;
    MonsterTextures fSphinx;
    MonsterTextures fTalos;

    MonsterTextures fSatyr;

    HeroTextures fAchilles;
    HeroTextures fAtalanta;
    HeroTextures fBellerophon;
    HeroTextures fHercules;
    HeroTextures fJason;
    HeroTextures fOdysseus;
    HeroTextures fPerseus;
    HeroTextures fTheseus;

    BasicCharacterTextures fScholar;
    BasicCharacterTextures fAstronomer;
    BasicCharacterTextures fInventor;
    BasicCharacterTextures fCurator;

    BasicCharacterTextures fChariotVendor;
    std::vector<TextureCollection> fChariot;

    BasicCharacterTextures fElephant;

    FightingCharacterTextures fEliteCitizen;

    RacingHorseTextures fRacingHorse1;
    RacingHorseTextures fRacingHorse2;
    RacingHorseTextures fRacingHorse3;
    RacingHorseTextures fRacingHorse4;
};

#endif // CHARACTER_TEXTURES_H
