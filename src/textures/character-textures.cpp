#include "character-textures.h"

#include "sprite-loader.h"

#include "offsets/SprMain.h"
#include "offsets/Zeus_Greek.h"
#include "offsets/SprAmbient.h"

#include "spriteData/peddler30.h"

#include "spriteData/actor30.h"

#include "spriteData/ox30.h"

#include "spriteData/porter30.h"

#include "spriteData/oxHandler30.h"

#include "spriteData/trailer30.h"

#include "spriteData/settlers130.h"

#include "spriteData/settlers230.h"

#include "spriteData/fireFighter30.h"

#include "spriteData/watchman30.h"

#include "spriteData/goatherd30.h"

#include "spriteData/priest30.h"

#include "spriteData/bronzeMiner30.h"

#include "spriteData/artisan30.h"

#include "spriteData/foodVendor30.h"

#include "spriteData/fleeceVendor30.h"

#include "spriteData/oilVendor30.h"

#include "spriteData/wineVendor30.h"

#include "spriteData/armsVendor30.h"

#include "spriteData/horseVendor30.h"

#include "spriteData/fleecedSheep30.h"

#include "spriteData/horse30.h"

#include "spriteData/shepherd30.h"

#include "spriteData/marbleMiner30.h"

#include "spriteData/silverMiner30.h"

#include "spriteData/archer30.h"

#include "spriteData/lumberjack30.h"

#include "spriteData/taxCollector30.h"

#include "spriteData/transporter30.h"

#include "spriteData/grower30.h"

#include "spriteData/trader30.h"

#include "spriteData/waterDistributor30.h"

#include "spriteData/rockThrower30.h"

#include "spriteData/hoplite30.h"

#include "spriteData/horseman30.h"

#include "spriteData/healer30.h"

#include "spriteData/nudeSheep30.h"

#include "spriteData/cart30.h"

#include "spriteData/boar30.h"

#include "spriteData/gymnast30.h"

#include "spriteData/competitor30.h"

#include "spriteData/goat30.h"

#include "spriteData/wolf30.h"

#include "spriteData/hunter30.h"

#include "spriteData/philosopher30.h"

#include "spriteData/fishingBoat30.h"

#include "spriteData/urchinGatherer30.h"

#include "spriteData/tradeBoat30.h"

#include "spriteData/greekHoplite30.h"

#include "spriteData/greekHorseman30.h"

#include "spriteData/greekRockThrower30.h"

#include "spriteData/foodCart30.h"

#include "spriteData/donkey30.h"

#include "spriteData/banners30.h"

#include "spriteData/poseidonBannerTops30.h"

#include "sprite-loader.h"

CharacterTextures::CharacterTextures(const int tileW, const int tileH,
                                       SDL_Renderer* const renderer) :
    fTileW(tileW), fTileH(tileH),
    fRenderer(renderer),

    fPeddler(renderer),
    fActor(renderer),
    fTaxCollector(renderer),
    fWaterDistributor(renderer),
    fWatchman(renderer),
    fFireFighter(renderer),
    fHealer(renderer),
    fGymnast(renderer),
    fCompetitor(renderer),
    fPhilosopher(renderer),

    fOx(renderer),
    fOxHandler(renderer),

    fEmptyTrailer(renderer),
    fWoodTrailer1(renderer),
    fWoodTrailer2(renderer),
    fMarbleTrailer1(renderer),
    fMarbleTrailer2(renderer),
    fBlackMarbleTrailer1(renderer),
    fBlackMarbleTrailer2(renderer),
    fSculptureTrailer(renderer),
    fEmptyBigTrailer(renderer),
    fMarbleBigTrailer(renderer),
    fBlackMarbleBigTrailer(renderer),

    fMarbleMiner(renderer),
    fSilverMiner(renderer),
    fBronzeMiner(renderer),
    fOrichalcMiner(renderer),
    fLumberjack(renderer),

    fArtisan(renderer),

    fHunter(renderer),
    fDeerHunter(renderer),

    fShepherd(renderer),
    fGoatherd(renderer),

    fFoodVendor(renderer),
    fFleeceVendor(renderer),
    fOilVendor(renderer),
    fWineVendor(renderer),
    fArmsVendor(renderer),
    fHorseVendor(renderer),

    fGrower(renderer),

    fBoar(renderer),
    fDeer(renderer),
    fWolf(renderer),

    fCattle1(renderer),
    fCattle2(renderer),
    fCattle3(renderer),
    fBull(renderer),
    fButcher(renderer),

    fGoat(renderer),
    fNudeSheep(renderer),
    fFleecedSheep(renderer),
    fHorse(renderer),

    fSettlers1(renderer),
    fSettlers2(renderer),

    fTransporter(renderer),

    fEmptyCart(renderer),

    fOrangeTender(renderer),

    fArcher(renderer),
    fPoseidonTowerArcher(renderer),

    fRockThrower(renderer),
    fHoplite(renderer),
    fHorseman(renderer),

    fHoplitePoseidon(renderer),
    fArcherPoseidon(renderer),

    fGreekRockThrower(renderer),
    fGreekHoplite(renderer),
    fGreekHorseman(renderer),

    fAmazonSpear(renderer),
    fAmazonArcher(renderer),

    fTrojanHoplite(renderer),
    fTrojanSpearthrower(renderer),
    fTrojanHorseman(renderer),

    fCentaurHorseman(renderer),
    fCentaurArcher(renderer),

    fEgyptianHoplite(renderer),
    fEgyptianArcher(renderer),

    fMayanHoplite(renderer),
    fMayanArcher(renderer),

    fPhoenicianHorseman(renderer),
    fPhoenicianArcher(renderer),

    fOceanidHoplite(renderer),
    fOceanidSpearthrower(renderer),

    fPersianHoplite(renderer),
    fPersianHorseman(renderer),
    fPersianArcher(renderer),

    fAtlanteanHoplite(renderer),
    fAtlanteanArcher(renderer),

    fBannerRod(renderer),
    fBannerTops(renderer),
    fPoseidonBannerTops(renderer),

    fPriest(renderer),

    fTrader(renderer),
    fDonkey(renderer),

    fPorter(renderer),

    fFishingBoat(renderer),
    fUrchinGatherer(renderer),

    fTradeBoat(renderer),
    fTrireme(renderer),
    fEnemyBoat(renderer),

    fDisgruntled(renderer),
    fSick(renderer),
    fHomeless(renderer),

    fKraken(renderer),
    fScylla(renderer),

    fScholar(renderer),
    fAstronomer(renderer),
    fInventor(renderer),
    fCurator(renderer),

    fChariotVendor(renderer),

    fElephant(renderer),

    fEliteCitizen(renderer),

    fRacingHorse1(renderer),
    fRacingHorse2(renderer),
    fRacingHorse3(renderer),
    fRacingHorse4(renderer) {

}

void CharacterTextures::loadAll() {
    loadPeddler();
    loadActor();
    loadOx();
    loadPorter();
    loadOxHandler();
    loadTrailer();
    loadSettlers();
    loadFireFighter();
    loadWatchman();
    loadGoatherd();
    loadBronzeMiner();
    loadOrichalcMiner();
    loadArtisan();
    loadFoodVendor();
    loadFleeceVendor();
    loadOilVendor();
    loadWineVendor();
    loadArmsVendor();
    loadHorseVendor();
    loadSheep();
    loadHorse();
    loadShepherd();
    loadMarbleMiner();
    loadSilverMiner();
    loadArcher();
    loadPoseidonTowerArcher();
    loadLumberjack();
    loadTaxCollector();
    loadTransporter();
    loadGrower();
    loadOrangeTender();
    loadTrader();
    loadWaterDistributor();

    loadRockThrower();
    loadHoplite();
    loadHorseman();

    loadAmazonSpear();
    loadAmazonArcher();

    loadTrojanHoplite();
    loadTrojanSpearthrower();
    loadTrojanHorseman();

    loadCentaurHorseman();
    loadCentaurArcher();

    loadPersianHoplite();
    loadPersianHorseman();
    loadPersianArcher();

    loadEgyptianHoplite();
    loadEgyptianChariot();
    loadEgyptianArcher();

    loadMayanHoplite();
    loadMayanArcher();

    loadOceanidHoplite();
    loadOceanidSpearthrower();

    loadPhoenicianHorseman();
    loadPhoenicianArcher();

    loadAtlanteanHoplite();
    loadAtlanteanChariot();
    loadAtlanteanArcher();

    loadAresWarrior();

    loadHealer();
    loadCart();
    loadOrichalcCart();
    loadOrangesCart();
    loadBlackMarbleTrailer();
    loadBoar();
    loadGymnast();
    loadCompetitor();
    loadGoat();
    loadWolf();
    loadHunter();
    loadDeerHunter();
    loadPhilosopher();
    loadUrchinGatherer();
    loadFishingBoat();
    loadTradeBoat();
    loadTrireme();
    loadTriremeOverlay();
    loadEnemyBoat();
    loadDeer();
    loadGreekHoplite();
    loadGreekHorseman();
    loadGreekRockThrower();
    loadDonkey();

    loadDisgruntled();
    loadSick();
    loadHomeless();

    loadCalydonianBoar();
    loadCerberus();
    loadChimera();
    loadCyclops();
    loadDragon();
    loadEchidna();
    loadHarpie();
    loadHector();
    loadHydra();
    loadKraken();
    loadMaenads();
    loadMedusa();
    loadMinotaur();
    loadScylla();
    loadSphinx();
    loadTalos();

    loadAchilles();
    loadAtalanta();
    loadBellerophon();
    loadHeracles();
    loadJason();
    loadOdysseus();
    loadPerseus();
    loadTheseus();

    loadScholar();
    loadAstronomer();
    loadInventor();
    loadCurator();

    loadHoplitePoseidon();
    loadArcherPoseidon();
    loadChariotPoseidon();

    loadCattle();
    loadBull();
    loadButcher();

    loadChariotVendor();

    loadChariot();

    loadElephant();

    loadSatyr();

    loadBanners();

    loadEliteCitizen();
}

void loadBasicTexture(BasicCharacterTextures& tex,
                      const int start,
                      SpriteLoader& loader) {
    loader.loadSkipFlipped(start, start, start + 96, tex.fWalk);

    for(int i = start + 96; i < start + 104; i++) {
        loader.load(start, i, tex.fDie);
    }
}

void CharacterTextures::loadPeddler() {
    if(fPeddlerLoaded) return;
    fPeddlerLoaded = true;
    const auto& sds = ePeddlerSpriteData30;
    SpriteLoader loader(fTileH, "peddler", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fPeddler, 1, loader);
}

void CharacterTextures::loadPriest() {
    if(fPriestLoaded) return;
    fPriestLoaded = true;
    const auto& sds = ePriestSpriteData30;
    SpriteLoader loader(fTileH, "priest", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fPriest, 8105, loader);
}

void CharacterTextures::loadActor() {
    if(fActorLoaded) return;
    fActorLoaded = true;
    const auto& sds = eActorSpriteData30;
    SpriteLoader loader(fTileH, "actor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fActor, 105, loader);
}

void CharacterTextures::loadOx() {
    if(fOxLoaded) return;
    fOxLoaded = true;

    const auto& sds = eOxSpriteData30;
    SpriteLoader loader(fTileH, "ox", sds,
                         &eSprMainOffset, fRenderer);
    loader.loadSkipFlipped(209, 209, 305, fOx.fWalk);

    for(auto& coll : fOx.fWalk) {
        const int iMax = coll.size();
        for(int i = 0; i < iMax; i++) {
            auto& tex = coll.getTexture(i);
            tex->setOffset(tex->offsetX(),
                           tex->offsetY() + 4);
        }
    }

    for(int i = 497; i < 505; i++) {
        loader.load(209, i, fOx.fDie);
    }
}

void CharacterTextures::loadPorter() {
    if(fPorterLoaded) return;
    fPorterLoaded = true;

    const auto& sds = ePorterSpriteData30;
    SpriteLoader loader(fTileH, "porter", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fPorter, 1233, loader);
}

void CharacterTextures::loadOxHandler() {
    if(fOxHandlerLoaded) return;
    fOxHandlerLoaded = true;

    const auto& sds = eOxHandlerSpriteData30;
    SpriteLoader loader(fTileH, "oxHandler", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fOxHandler, 1337, loader);
}

void CharacterTextures::loadTrailer() {
    if(fTrailerLoaded) return;
    fTrailerLoaded = true;
    const auto& sds = eTrailerSpriteData30;
    SpriteLoader loader(fTileH, "trailer", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadTrailer(2991, 2991, 2999, fEmptyTrailer, -7);
    loader.loadTrailer(2991, 2999, 3007, fWoodTrailer1, -7);
    loader.loadTrailer(2991, 3007, 3015, fWoodTrailer2, -7);
    loader.loadTrailer(2991, 3015, 3023, fMarbleTrailer1, -7);
    loader.loadTrailer(2991, 3023, 3031, fMarbleTrailer2, -7);
    loader.loadTrailer(2991, 3031, 3039, fSculptureTrailer, -7);
    loader.loadTrailer(2991, 3039, 3047, fEmptyBigTrailer, -4);
    loader.loadTrailer(2991, 3047, 3055, fMarbleBigTrailer, -4);

    loadBlackMarbleTrailer();
}

void CharacterTextures::loadSettlers() {
    if(fSettlersLoaded) return;
    fSettlersLoaded = true;
    {
        const auto& sds = eSettlers1SpriteData30;
        SpriteLoader loader(fTileH, "settlers1", sds,
                             &eSprMainOffset, fRenderer);
        loadBasicTexture(fSettlers1, 505, loader);
    }
    {
        const auto& sds = eSettlers2SpriteData30;
        SpriteLoader loader(fTileH, "settlers2", sds,
                             &eSprMainOffset, fRenderer);
        loadBasicTexture(fSettlers2, 1793, loader);
    }
}

void CharacterTextures::loadFireFighter() {
    if(fFireFighterLoaded) return;
    fFireFighterLoaded = true;
    const auto& sds = eFireFighterSpriteData30;
    SpriteLoader loader(fTileH, "fireFighter", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(609, 609, 705,
                           fFireFighter.fWalk);
    loader.loadSkipFlipped(609, 705, 801,
                           fFireFighter.fCarry);
    loader.loadSkipFlipped(609, 809, 1129,
                           fFireFighter.fPutOut);

    for(int i = 801; i < 809; i++) {
        loader.load(609, i, fFireFighter.fDie);
    }
}

void CharacterTextures::loadWatchman() {
    if(fWatchmanLoaded) return;
    fWatchmanLoaded = true;
    const auto& sds = eWatchmanSpriteData30;
    SpriteLoader loader(fTileH, "watchman", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(2209, 2209, 2305, fWatchman.fWalk);
    loader.loadSkipFlipped(2209, 2313, 2377, fWatchman.fFight);

    for(int i = 2305; i < 2313; i++) {
        loader.load(2209, i, fWatchman.fDie);
    }
}

void CharacterTextures::loadGoatherd() {
    if(fGoatherdLoaded) return;
    fGoatherdLoaded = true;
    const auto& sds = eGoatherdSpriteData30;
    SpriteLoader loader(fTileH, "goatherd", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(2377, 2377, 2473, fGoatherd.fWalk);
    for(int i = 2481; i < 2489; i++) {
        loader.load(2377, i, fGoatherd.fCollect);
    }
    loader.loadSkipFlipped(2377, 2489, 2585, fGoatherd.fCarry);

    for(int i = 2473; i < 2481; i++) {
        loader.load(2377, i, fGoatherd.fDie);
    }
    for(int i = 2585; i < 2595; i++) {
        loader.load(2377, i, fGoatherd.fFight);
    }
}

void CharacterTextures::loadBronzeMiner() {
    if(fBronzeMinerLoaded) return;
    fBronzeMinerLoaded = true;
    const auto& sds = eBronzeMinerSpriteData30;
    SpriteLoader loader(fTileH, "bronzeMiner", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(2595, 2595, 2691, fBronzeMiner.fWalk);
    loader.loadSkipFlipped(2595, 2711, 2807, fBronzeMiner.fCarry);
    loader.loadSkipFlipped(2595, 2807, 2887, fBronzeMiner.fCollect);

    for(int i = 2691; i < 2699; i++) {
        loader.load(2595, i, fBronzeMiner.fDie);
    }
}

void CharacterTextures::loadArtisan() {
    if(fArtisanLoaded) return;
    fArtisanLoaded = true;
    const auto& sds = eArtisanSpriteData30;
    SpriteLoader loader(fTileH, "artisan", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(1545, 1545, 1641, fArtisan.fWalk);
    loader.loadSkipFlipped(1545, 1649, 1721, fArtisan.fBuild);
    loader.loadSkipFlipped(1545, 1721, 1793, fArtisan.fBuildStanding);

    for(int i = 1641; i < 1649; i++) {
        loader.load(1545, i, fArtisan.fDie);
    }
}

void CharacterTextures::loadFoodVendor() {
    if(fFoodVendorLoaded) return;
    fFoodVendorLoaded = true;
    const auto& sds = eFoodVendorSpriteData30;
    SpriteLoader loader(fTileH, "foodVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fFoodVendor, 2887, loader);
}

void CharacterTextures::loadFleeceVendor() {
    if(fFleeceVendorLoaded) return;
    fFleeceVendorLoaded = true;
    const auto& sds = eFleeceVendorSpriteData30;
    SpriteLoader loader(fTileH, "fleeceVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fFleeceVendor, 1897, loader);
}

void CharacterTextures::loadOilVendor() {
    if(fOilVendorLoaded) return;
    fOilVendorLoaded = true;
    const auto& sds = eOilVendorSpriteData30;
    SpriteLoader loader(fTileH, "oilVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fOilVendor, 5297, loader);
}

void CharacterTextures::loadWineVendor() {
    if(fWineVendorLoaded) return;
    fWineVendorLoaded = true;
    const auto& sds = eWineVendorSpriteData30;
    SpriteLoader loader(fTileH, "wineVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fWineVendor, 5401, loader);
}

void CharacterTextures::loadArmsVendor() {
    if(fArmsVendorLoaded) return;
    fArmsVendorLoaded = true;
    const auto& sds = eArmsVendorSpriteData30;
    SpriteLoader loader(fTileH, "armsVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fArmsVendor, 2105, loader);
}

void CharacterTextures::loadHorseVendor() {
    if(fHorseVendorLoaded) return;
    fHorseVendorLoaded = true;
    const auto& sds = eHorseVendorSpriteData30;
    SpriteLoader loader(fTileH, "horseVendor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fHorseVendor, 1129, loader);
}

void CharacterTextures::loadSheep() {
    if(fSheepLoaded) return;
    fSheepLoaded = true;

    {
        const auto& sds = eFleecedSheepSpriteData30;
        SpriteLoader loader(fTileH, "fleecedSheep", sds,
                             &eSprMainOffset, fRenderer);
        loader.loadSkipFlipped(3183, 3183, 3279, fFleecedSheep.fWalk);
        loader.loadSkipFlipped(3183, 3287, 3351, fFleecedSheep.fFight);
        loader.loadSkipFlipped(3183, 3351, 3415, fFleecedSheep.fLayDown);

        for(int i = 3279; i < 3287; i++) {
            loader.load(3183, i, fFleecedSheep.fDie);
        }
    }

    {
        const auto& sds = eNudeSheepSpriteData30;
        SpriteLoader loader(fTileH, "nudeSheep", sds,
                             &eSprMainOffset, fRenderer);
        loader.loadSkipFlipped(7873, 7873, 7969, fNudeSheep.fWalk);
        loader.loadSkipFlipped(7873, 7977, 8041, fNudeSheep.fFight);
        loader.loadSkipFlipped(7873, 8041, 8105, fNudeSheep.fLayDown);

        for(int i = 7969; i < 7977; i++) {
            loader.load(7873, i, fNudeSheep.fDie);
        }
    }
}

void CharacterTextures::loadHorse() {
    if(fHorseLoaded) return;
    fHorseLoaded = true;
    const auto& sds = eHorseSpriteData30;
    SpriteLoader loader(fTileH, "horse", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(5001, 5001, 5097, fHorse.fWalk);
    loader.loadSkipFlipped(5001, 5105, 5297, fHorse.fStand);

    for(int i = 5097; i < 5105; i++) {
        loader.load(5001, i, fHorse.fDie);
    }
}

void CharacterTextures::loadShepherd() {
    if(fShepherdLoaded) return;
    fShepherdLoaded = true;
    const auto& sds = eShepherdSpriteData30;
    SpriteLoader loader(fTileH, "shepherd", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(3415, 3415, 3511, fShepherd.fWalk);
    for(int i = 3519; i < 3531; i++) {
        loader.load(3415, i, fShepherd.fCollect);
    }
    loader.loadSkipFlipped(3415, 3531, 3627, fShepherd.fCarry);

    for(int i = 3627; i < 3637; i++) {
        loader.load(3415, i, fShepherd.fFight);
    }
    for(int i = 3511; i < 3519; i++) {
        loader.load(3415, i, fShepherd.fDie);
    }
}

void CharacterTextures::loadMarbleMiner() {
    if(fMarbleMinerLoaded) return;
    fMarbleMinerLoaded = true;
    const auto& sds = eMarbleMinerSpriteData30;
    SpriteLoader loader(fTileH, "marbleMiner", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(11044, 11044, 11140, fMarbleMiner.fWalk);
    loader.loadSkipFlipped(11044, 11148, 11228, fMarbleMiner.fCollect);

    for(int i = 11140; i < 11148; i++) {
        loader.load(11044, i, fMarbleMiner.fDie);
    }
}

void CharacterTextures::loadSilverMiner() {
    if(fSilverMinerLoaded) return;
    fSilverMinerLoaded = true;
    const auto& sds = eSilverMinerSpriteData30;
    SpriteLoader loader(fTileH, "silverMiner", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(3741, 3741, 3837, fSilverMiner.fWalk);
    loader.loadSkipFlipped(3741, 3857, 3953, fSilverMiner.fCarry);
    loader.loadSkipFlipped(3741, 3953, 4033, fSilverMiner.fCollect);

    for(int i = 3837; i < 3845; i++) {
        loader.load(3741, i, fSilverMiner.fDie);
    }
}

void CharacterTextures::loadArcher() {
    if(fArcherLoaded) return;
    fArcherLoaded = true;
    const auto& sds = eArcherSpriteData30;
    SpriteLoader loader(fTileH, "archer", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(4033, 4033, 4129, fArcher.fWalk);
    loader.loadSkipFlipped(4033, 4137, 4233, fArcher.fFight);
    loader.loadSkipFlipped(4033, 4233, 4329, fArcher.fPatrol);

    for(int i = 4129; i < 4137; i++) {
        loader.load(4033, i, fArcher.fDie);
    }
}

void CharacterTextures::loadLumberjack() {
    if(fLumberjackLoaded) return;
    fLumberjackLoaded = true;
    const auto& sds = eLumberjackSpriteData30;
    SpriteLoader loader(fTileH, "lumberjack", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(4329, 4329, 4425, fLumberjack.fWalk);
    loader.loadSkipFlipped(4329, 4433, 4529, fLumberjack.fCollect);
    loader.loadSkipFlipped(4329, 4529, 4625, fLumberjack.fCarry);

    for(int i = 4425; i < 4433; i++) {
        loader.load(4329, i, fLumberjack.fDie);
    }
}

void CharacterTextures::loadTaxCollector() {
    if(fTaxCollectorLoaded) return;
    fTaxCollectorLoaded = true;
    const auto& sds = eTaxCollectorSpriteData30;
    SpriteLoader loader(fTileH, "taxCollector", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fTaxCollector, 4625, loader);
}

void CharacterTextures::loadTransporter() {
    if(fTransporterLoaded) return;
    fTransporterLoaded = true;
    const auto& sds = eTransporterSpriteData30;
    SpriteLoader loader(fTileH, "transporter", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fTransporter, 4729, loader);
}

void CharacterTextures::loadGrower() {
    if(fGrowerLoaded) return;
    fGrowerLoaded = true;
    {
        const auto& sds = eGrowerSpriteData30;
        SpriteLoader loader(fTileH, "grower", sds,
                             &eSprMainOffset, fRenderer);

        loader.loadSkipFlipped(5505, 5505, 5601, fGrower.fWalk);
        loader.loadSkipFlipped(5505, 5609, 5689, fGrower.fWorkOnGrapes);
        loader.loadSkipFlipped(5505, 5689, 5769, fGrower.fWorkOnOlives);
        loader.loadSkipFlipped(5505, 5769, 5849, fGrower.fCollectGrapes);
        loader.loadSkipFlipped(5505, 5849, 5929, fGrower.fCollectOlives);

        for(int i = 5601; i < 5609; i++) {
            loader.load(5505, i, fGrower.fDie);
        }
    }
    loadOrangeTender();
}

void CharacterTextures::loadTrader() {
    if(fTraderLoaded) return;
    fTraderLoaded = true;
    const auto& sds = eTraderSpriteData30;
    SpriteLoader loader(fTileH, "trader", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fTrader, 5929, loader);
}

void CharacterTextures::loadWaterDistributor() {
    if(fWaterDistributorLoaded) return;
    fWaterDistributorLoaded = true;
    const auto& sds = eWaterDistributorSpriteData30;
    SpriteLoader loader(fTileH, "waterDistributor", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fWaterDistributor, 6737, loader);
}

void CharacterTextures::loadRockThrower() {
    if(fRockThrowerLoaded) return;
    fRockThrowerLoaded = true;
    const auto& sds = eRockThrowerSpriteData30;
    SpriteLoader loader(fTileH, "rockThrower", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(6841, 6841, 6937, fRockThrower.fWalk);
    loader.loadSkipFlipped(6841, 6945, 7041, fRockThrower.fFight2);
    loader.loadSkipFlipped(6841, 7041, 7105, fRockThrower.fFight);

    for(int i = 6937; i < 6945; i++) {
        loader.load(6841, i, fRockThrower.fDie);
    }
}

void CharacterTextures::loadHoplite() {
    if(fHopliteLoaded) return;
    const auto& sds = eHopliteSpriteData30;
    fHopliteLoaded = true;
    SpriteLoader loader(fTileH, "hoplite", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(7105, 7105, 7201, fHoplite.fWalk);
    loader.loadSkipFlipped(7105, 7209, 7273, fHoplite.fFight);

    for(int i = 7201; i < 7209; i++) {
        loader.load(7105, i, fHoplite.fDie);
    }
}

void CharacterTextures::loadHorseman() {
    if(fHorsemanLoaded) return;
    fHorsemanLoaded = true;

    const auto& sds = eHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "horseman", sds,
                         &eSprMainOffset, fRenderer);
    loader.loadSkipFlipped(7273, 7273, 7369, fHorseman.fWalk);
    loader.loadSkipFlipped(7273, 7377, 7473, fHorseman.fFight);

    for(int i = 7369; i < 7377; i++) {
        loader.load(7273, i, fHorseman.fDie);
    }
}

void CharacterTextures::loadHealer() {
    if(fHealerLoaded) return;
    fHealerLoaded = true;
    const auto& sds = eHealerSpriteData30;
    SpriteLoader loader(fTileH, "healer", sds,
                         &eSprMainOffset, fRenderer);

    loadBasicTexture(fHealer, 7473, loader);
}

void CharacterTextures::loadCart() {
    if(fCartLoaded) return;
    fCartLoaded = true;
    {
        const auto& sds = eCartSpriteData30;
        SpriteLoader loader(fTileH, "cart", sds,
                             &eSprMainOffset, fRenderer);

        loader.loadSkipFlipped(8428, 8428, 8436, fEmptyCart);
        loader.loadSkipFlipped(8428, 8436, 8460, fUrchinCart);
        loader.loadSkipFlipped(8428, 8460, 8484, fFishCart);
        loader.loadSkipFlipped(8428, 8484, 8508, fMeatCart);
        loader.loadSkipFlipped(8428, 8508, 8532, fCheeseCart);
        loader.loadSkipFlipped(8428, 8532, 8556, fCarrotsCart);
        loader.loadSkipFlipped(8428, 8556, 8580, fOnionsCart);
        loader.loadSkipFlipped(8428, 8580, 8604, fWheatCart);
        loader.loadSkipFlipped(8428, 8604, 8620, fBronzeCart);
        loader.loadSkipFlipped(8428, 8620, 8644, fGrapesCart);
        loader.loadSkipFlipped(8428, 8644, 8668, fOlivesCart);
        loader.loadSkipFlipped(8428, 8668, 8684, fFleeceCart);
        loader.loadSkipFlipped(8428, 8684, 8700, fArmorCart);
        loader.loadSkipFlipped(8428, 8700, 8716, fOliveOilCart);
        loader.loadSkipFlipped(8428, 8716, 8732, fWineCart);
    }
    loadOrangesCart();
    loadOrichalcCart();
}

void CharacterTextures::loadBoar() {
    if(fBoarLoaded) return;
    fBoarLoaded = true;
    const auto& sds = eBoarSpriteData30;
    SpriteLoader loader(fTileH, "boar", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(10124, 10124, 10220, fBoar.fWalk);
    loader.loadSkipFlipped(10124, 10228, 10356, fBoar.fFight);
    loader.loadSkipFlipped(10124, 10356, 10420, fBoar.fLayDown);

    for(int i = 10220; i < 10228; i++) {
        loader.load(10124, i, fBoar.fDie);
    }
}

void CharacterTextures::loadGymnast() {
    if(fGymnastLoaded) return;
    fGymnastLoaded = true;

    const auto& sds = eGymnastSpriteData30;
    SpriteLoader loader(fTileH, "gymnast", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fGymnast, 10588, loader);
}

void CharacterTextures::loadCompetitor() {
    if(fCompetitorLoaded) return;
    fCompetitorLoaded = true;

    const auto& sds = eCompetitorSpriteData30;
    SpriteLoader loader(fTileH, "competitor", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fCompetitor, 10692, loader);
}

void CharacterTextures::loadGoat() {
    if(fGoatLoaded) return;
    fGoatLoaded = true;

    const auto& sds = eGoatSpriteData30;
    SpriteLoader loader(fTileH, "goat", sds,
                         &eSprMainOffset, fRenderer);
    loader.loadSkipFlipped(11228, 11228, 11324, fGoat.fWalk);
    loader.loadSkipFlipped(11228, 11332, 11460, fGoat.fFight);
    loader.loadSkipFlipped(11228, 11460, 11524, fGoat.fLayDown);

    for(int i = 11324; i < 11332; i++) {
        loader.load(11228, i, fGoat.fDie);
    }
}

void CharacterTextures::loadWolf() {
    if(fWolfLoaded) return;
    fWolfLoaded = true;

    const auto& sds = eWolfSpriteData30;
    SpriteLoader loader(fTileH, "wolf", sds,
                         &eSprMainOffset, fRenderer);
    loader.loadSkipFlipped(11524, 11524, 11620, fWolf.fWalk);
    loader.loadSkipFlipped(11524, 11628, 11756, fWolf.fFight);
    loader.loadSkipFlipped(11524, 11756, 11820, fWolf.fLayDown);

    for(int i = 11620; i < 11628; i++) {
        loader.load(11524, i, fWolf.fDie);
    }
}

void CharacterTextures::loadHunter() {
    if(fHunterLoaded) return;
    fHunterLoaded = true;
    {

        const auto& sds = eHunterSpriteData30;
        SpriteLoader loader(fTileH, "hunter", sds,
                             &eSprMainOffset, fRenderer);
        loader.loadSkipFlipped(11820, 11820, 11916, fHunter.fWalk);
        loader.loadSkipFlipped(11820, 11924, 12019, fHunter.fCollect);
        loader.loadSkipFlipped(11820, 12032, 12128, fHunter.fCarry);

        for(int i = 11916; i < 11924; i++) {
            loader.load(11820, i, fHunter.fDie);
        }
    }
    loadDeerHunter();
}

void CharacterTextures::loadPhilosopher() {
    if(fPhilosopherLoaded) return;
    fPhilosopherLoaded = true;

    const auto& sds = ePhilosopherSpriteData30;
    SpriteLoader loader(fTileH, "philosopher", sds,
                         &eSprMainOffset, fRenderer);
    loadBasicTexture(fPhilosopher, 12128, loader);
}

void CharacterTextures::loadUrchinGatherer() {
    if(fUrchinGathererLoaded) return;
    fUrchinGathererLoaded = true;

    const auto& sds = eUrchinGathererSpriteData30;
    SpriteLoader loader(fTileH, "urchinGatherer", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(9508, 9508, 9604, fUrchinGatherer.fSwim);
    loader.loadSkipFlipped(9508, 9604, 9924, fUrchinGatherer.fCollect);
    loader.loadSkipFlipped(9508, 9924, 10020, fUrchinGatherer.fCarry);
    loader.loadSkipFlipped(9508, 10020, 10116, fUrchinGatherer.fDeposit);
    for(int i = 10116; i < 10124; i++) {
        loader.load(9508, i, fUrchinGatherer.fDie);
    }
}

void CharacterTextures::loadFishingBoat() {
    if(fFishingBoatLoaded) return;
    fFishingBoatLoaded = true;

    const auto& sds = eFishingBoatSpriteData30;
    SpriteLoader loader(fTileH, "fishingBoat", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadBoatSkipFlipped(10796, 10797, 10860, fFishingBoat.fSwim);
    loader.loadSkipFlipped(10796, 10860, 10940, fFishingBoat.fCollect);
    loader.loadSkipFlipped(10796, 10940, 10948, fFishingBoat.fStand);
    loader.loadSkipFlipped(10796, 10948, 11036, fFishingBoat.fDie);
}

void CharacterTextures::loadTradeBoat() {
    if(fTradeBoatLoaded) return;
    fTradeBoatLoaded = true;

    const auto& sds = eTradeBoatSpriteData30;
    SpriteLoader loader(fTileH, "tradeBoat", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadBoatSkipFlipped(10420, 10421, 10484, fTradeBoat.fSwim);
    loader.loadSkipFlipped(10420, 10484, 10580, fTradeBoat.fDie);
    loader.loadSkipFlipped(10420, 10580, 10588, fTradeBoat.fStand);
}

void CharacterTextures::loadGreekRockThrower() {
    if(fGreekRockThrowerLoaded) return;
    fGreekRockThrowerLoaded = true;

    const auto& sds = eGreekRockThrowerSpriteData30;
    SpriteLoader loader(fTileH, "greekRockThrower", sds,
                         &eZeus_GreekOffset, fRenderer);
    loader.loadSkipFlipped(369, 369, 465, fGreekRockThrower.fWalk);
    loader.loadSkipFlipped(369, 473, 569, fGreekRockThrower.fFight2);
    loader.loadSkipFlipped(369, 569, 633, fGreekRockThrower.fFight);

    for(int i = 465; i < 473; i++) {
        loader.load(369, i, fGreekRockThrower.fDie);
    }
}

void CharacterTextures::loadGreekHoplite() {
    if(fGreekHopliteLoaded) return;
    fGreekHopliteLoaded = true;

    const auto& sds = eGreekHopliteSpriteData30;
    SpriteLoader loader(fTileH, "greekHoplite", sds,
                         &eZeus_GreekOffset, fRenderer);
    loader.loadSkipFlipped(1, 1, 97, fGreekHoplite.fWalk);
    loader.loadSkipFlipped(1, 105, 169, fGreekHoplite.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fGreekHoplite.fDie);
    }
}

void CharacterTextures::loadGreekHorseman() {
    if(fGreekHorsemanLoaded) return;
    fGreekHorsemanLoaded = true;

    const auto& sds = eGreekHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "greekHorseman", sds,
                         &eZeus_GreekOffset, fRenderer);
    loader.loadSkipFlipped(169, 169, 265, fGreekHorseman.fWalk);
    loader.loadSkipFlipped(169, 273, 369, fGreekHorseman.fFight);

    for(int i = 265; i < 273; i++) {
        loader.load(169, i, fGreekHorseman.fDie);
    }
}

void CharacterTextures::loadDonkey() {
    if(fDonkeyLoaded) return;
    fDonkeyLoaded = true;

    const auto& sds = eDonkeySpriteData30;
    SpriteLoader loader(fTileH, "donkey", sds,
                         &eSprAmbientOffset, fRenderer);
    loadBasicTexture(fDonkey, 529, loader);
}

void CharacterTextures::loadBanners() {
    if(fBannersLoaded) return;
    fBannersLoaded = true;

    {
        const auto& sds = eBannersSpriteData30;
        SpriteLoader loader(fTileH, "banners", sds,
                             nullptr, fRenderer);

        for(int i = 1; i < 22; i++) {
            loader.load(1, i, fBannerRod);
        }

        int ban = 0;
        for(int i = 43; i < 204;) {
            fBanners.emplace_back(fRenderer);
            auto& bani = fBanners[ban++];
            for(int j = 0; j < 7; j++, i++) {
                loader.load(1, i, bani);
            }
        }

        for(int i = 204; i < 207; i++) {
            loader.load(1, i, fBannerTops);
        }
    }
    {
        const auto& sds = ePoseidonBannerTopsSpriteData30;
        SpriteLoader loader(fTileH, "poseidonBannerTops", sds,
                             nullptr, fRenderer);

        for(int i = 44; i < 47; i++) {
            loader.load(44, i, fPoseidonBannerTops);
        }
    }
}
