#ifndef SOUNDS_H
#define SOUNDS_H

#include "sound-vector.h"

#include "god-sounds.h"
#include "hero-sounds.h"
#include "monster-sounds.h"

class eTile;

class eSounds {
public:
    eSounds();

    static void loadButtonSound();

    static void load();
    static bool loaded();
    static void setGeneralVolume(const int volume);
    static void setVolume(const int volume);
    static void setVoiceVolume(const int volume);
    static void setEventVolume(const int volume);
    static void setAmbientVolume(const int volume);

    static void playButtonSound();

    static void playPlaceBuildingSound();

    static void playSoundForTile(eTile* const tile);
    static bool playSoundForBuilding(eBuilding* const b);
    static bool playSoundForBuilding(const eBuildingType type);
    static bool playSoundForCharacter(eCharacter* const c);
    static bool playSoundForTerrain(const eTerrain terrain);

    static void playEnvironmentSound();

    static void playBeachSound();
    static void playWaterSound();
    static void playRockySound();

    static void playBeautificationSound();
    static void playSanctuarySound();

    static void playFarmingSound();

    static void playCommonHousingSound();
    static void playEliteHousingSound();

    static void playTheatreSound();
    static void playDramaSound();
    static void playPhilosophySound();
    static void playGymnasiumSound();
    static void playStadiumSound();

    static void playBibliothekeSound();
    static void playUniversitySound();
    static void playObservatorySound();
    static void playInventorsWorkshopSound();
    static void playLaboratorySound();
    static void playMuseumSound();

    static void playArtisansGuildSound();

    static void playFountainSound();
    static void playInfirmarySound();
    static void playTaxesSound();
    static void playPalaceSound();
    static void playMaintananceSound();

    static void playFoundrySound();
    static void playMarbleSound();
    static void playMintSound();
    static void playTimberMillSound();

    static void playArmorySound();
    static void playHorseRanchSound();
    static void playChariotFactorySound();

    static void playHuntingSound();
    static void playBoarSound();
    static void playDeerSound();
    static void playWolfSound();

    static void playSheepSound();
    static void playGoatSound();
    static void playCattleSound();
    static void playCorralSound();
    static void playFruitLodgeSound();
    static void playGoatherdSound();
    static void playShearingSound();

    static void playStorageSound();

    static void playFoodVendorSound();
    static void playFleeceVendorSound();
    static void playOilVendorSound();
    static void playWineVendorSound();
    static void playArmsVendorSound();
    static void playHorseTrainerSound();
    static void playChariotVendorSound();

    static void playTradeSound();

    static void playFireSound();
    static void playCollapseSound();

    static void playFireballHitSound();

    static void playEarthquakeSound();
    static void playLavaSound();

    static void playCopperMinerSound();
    static void playSilverMinerSound();
    static void playStoneCutterSound();
    static void playTreeCutterSound();

    static void playArtisanSound();

    static void playSeaSound();

    static void playGodSound(const eGodType g,
                             const eGodSound s);
    static void playHeroSound(const eHeroType h,
                              const eHeroSound s);
    static void playMonsterSound(const eMonsterType m,
                                 const eMonsterSound s);

    static void playAttackSound(eCharacter* const c);
    static void playTowerAttackSound();
    static bool canPlayCombatSound(eCharacter* const c);
    static void playDieSound(eCharacter* const c);
    static void playHitSound(eCharacter* const c);

    static eSoundVector* getCharacterVoices(eCharacter * const c);
private:
    void loadImpl();
    static void playAttackSound(const eCharacterType type);

    static eSounds* sInstance;

    bool mLoaded{false};

    eSoundVector mButton;
    eSoundVector mPlaceBuilding;

    eSoundVector mEnvironment;

    eSoundVector mMaintenance;
    eSoundVector mCommonHousing;
    eSoundVector mEliteHousing;
    eSoundVector mFarming;
    eSoundVector mCorral;
    eSoundVector mFruitLodge;
    eSoundVector mOrchard;
    eSoundVector mSheepFarm;
    eSoundVector mGoatFarm;
    eSoundVector mSea;
    eSoundVector mTriremeWharf;
    eSoundVector mHunting;
    eSoundVector mTimberMill;
    eSoundVector mMarble;
    eSoundVector mMint;
    eSoundVector mFoundry;
    eSoundVector mWorkshops;
    eSoundVector mStorage;
    eSoundVector mAgoraFood;
    eSoundVector mAgoraFleece;
    eSoundVector mAgoraOil;
    eSoundVector mAgoraWine;
    eSoundVector mAgoraArms;
    eSoundVector mAgoraHorse;
    eSoundVector mAgoraChariot;
    eSoundVector mTrade;
    eSoundVector mTaxes;
    eSoundVector mPalace;
    eSoundVector mDefensive;
    eSoundVector mInfirmary;
    eSoundVector mTheatre;
    eSoundVector mDrama;
    eSoundVector mPhilosophy;
    eSoundVector mGymnasium;
    eSoundVector mStadium;
    eSoundVector mSanctuary;
    eSoundVector mArmory;
    eSoundVector mHorseRanch;
    eSoundVector mBeautification;
    eSoundVector mFountain;
    eSoundVector mArtisansGuild;

    // terrain
    eSoundVector mMeadow;
    eSoundVector mFarmland;
    eSoundVector mBeach;
    eSoundVector mRocky;
    eSoundVector mVegetation;
    eSoundVector mWater;
    // animals
    eSoundVector mBoar;
    eSoundVector mGoat;
    eSoundVector mWolf;
    eSoundVector mSheep;
    eSoundVector mCattle;
    eSoundVector mDeer;

    // collectors
    eSoundVector mCopperMiner;
    eSoundVector mSilverMiner;
    eSoundVector mTreeCutter;
    eSoundVector mStoneCutter;

    eSoundVector mArtisan;

    // science
    eSoundVector mBibliotheke;
    eSoundVector mUniversity;
    eSoundVector mObservatory;
    eSoundVector mInventorsWorkshop;
    eSoundVector mLaboratory;
    eSoundVector mMuseum;

    eSoundVector mChariotFactory;

    // rockthrower
    eSoundVector mActorDie;
    eSoundVector mActorHit;

    eSoundVector mRockthrowerAttack;
    eSoundVector mRockthrowerDie;
    eSoundVector mRockthrowerHit;

    eSoundVector mArcherAttack;
    eSoundVector mArcherHit;
    eSoundVector mArcherDie;

    eSoundVector mOutlawHit;
    eSoundVector mOutlawDie;
    eSoundVector mOutlawAttack;

    eSoundVector mAresWarriorAttack;
    eSoundVector mAresWarriorHit;
    eSoundVector mAresWarriorDie;

    eSoundVector mTrojanHorseAttack;
    eSoundVector mTrojanHorseDie;
    eSoundVector mTrojanHorseHit;

    // Civilization-specific cavalry sounds
    eSoundVector mPersianCavalryAttack;
    eSoundVector mPhoenicianCavalryAttack;
    eSoundVector mEgyptianChariotAttack;
    eSoundVector mAtlanteanChariotAttack;

    // Civilization-specific infantry/archer sounds
    eSoundVector mAmazonWarriorAttack;
    eSoundVector mAmazonWarriorDie;
    eSoundVector mAmazonWarriorHit;

    eSoundVector mAmazonArcherDie;
    eSoundVector mAmazonArcherHit;

    eSoundVector mCentaurArcherDie;
    eSoundVector mCentaurArcherHit;
    eSoundVector mCentaurHorsemanDie;
    eSoundVector mCentaurHorsemanHit;
    eSoundVector mCentaurSwordDie;
    eSoundVector mCentaurSwordHit;

    eSoundVector mEgyptianArcherDie;
    eSoundVector mEgyptianArcherHit;
    eSoundVector mEgyptianChariotDie;
    eSoundVector mEgyptianChariotHit;
    eSoundVector mEgyptianHopliteAttack;
    eSoundVector mEgyptianHopliteDie;
    eSoundVector mEgyptianHopliteHit;

    eSoundVector mAtlanteanArcherDie;
    eSoundVector mAtlanteanArcherHit;
    eSoundVector mAtlanteanChariotDie;
    eSoundVector mAtlanteanChariotHit;
    eSoundVector mAtlanteanHopliteAttack;
    eSoundVector mAtlanteanHopliteDie;
    eSoundVector mAtlanteanHopliteHit;

    eSoundVector mAtlanteanMarinesAttack;
    eSoundVector mAtlanteanMarinesDie;
    eSoundVector mAtlanteanMarinesHit;
    eSoundVector mAtlanteanSentryDie;
    eSoundVector mAtlanteanSentryHit;

    eSoundVector mMayanArcherDie;
    eSoundVector mMayanArcherHit;
    eSoundVector mMayanHopliteAttack;
    eSoundVector mMayanHopliteDie;
    eSoundVector mMayanHopliteHit;

    eSoundVector mPhoenicianArcherDie;
    eSoundVector mPhoenicianArcherHit;
    eSoundVector mPhoenicianHorsemanDie;
    eSoundVector mPhoenicianHorsemanHit;

    eSoundVector mPersianArcherDie;
    eSoundVector mPersianArcherHit;
    eSoundVector mPersianHorsemanDie;
    eSoundVector mPersianHorsemanHit;
    eSoundVector mPersianHopliteAttack;
    eSoundVector mPersianHopliteDie;
    eSoundVector mPersianHopliteHit;

    // Missing sound vectors
    eSoundVector mElephantDie;
    eSoundVector mElephantHit;
    eSoundVector mNoblemanAttack;
    eSoundVector mNoblemanDie;
    eSoundVector mNoblemanHit;
    eSoundVector mPersianMissileDie;
    eSoundVector mPersianMissileHit;

    eSoundVector mMerchantInfantryAttack;
    eSoundVector mMerchantInfantryDie;
    eSoundVector mMerchantInfantryHit;
    eSoundVector mMerchantSpearAttack;
    eSoundVector mMerchantSpearDie;
    eSoundVector mMerchantSpearHit;

    eSoundVector mTrojanSpearthrowerDie;
    eSoundVector mTrojanSpearthrowerHit;
    eSoundVector mTrojanSpearthrowerAttack;

    eSoundVector mTrojanSwordDie;
    eSoundVector mTrojanSwordHit;
    eSoundVector mTrojanSwordAttack;

    eSoundVector mBoarAttack;
    eSoundVector mBoarDie;
    eSoundVector mBoarHit;

    eSoundVector mBoarHunterAttack;
    eSoundVector mBoarHunterDie;
    eSoundVector mBoarHunterHit;

    eSoundVector mDeerAttack;
    eSoundVector mDeerDie;
    eSoundVector mDeerHit;

    eSoundVector mWolfAttack;
    eSoundVector mWolfDie;
    eSoundVector mWolfHit;

    eSoundVector mSheepDie;
    eSoundVector mSheepHit;

    eSoundVector mCattleAttack;
    eSoundVector mCattleDie;
    eSoundVector mCattleHit;

    eSoundVector mGoatAttack;
    eSoundVector mGoatDie;
    eSoundVector mGoatHit;

    eSoundVector mHopliteDie;
    eSoundVector mHopliteAttack;
    eSoundVector mHopliteHit;

    eSoundVector mGenDie;
    eSoundVector mGenHit;

    // events
    eSoundVector mFire;
    eSoundVector mCollapse;

    eSoundVector mFireballHit;

    eSoundVector mEarthquake;
    eSoundVector mLava;

    eGodSounds mAphrodite{"Aph", "aphrodite"};
    eGodSounds mApollo{"Apo", "apollo"};
    eGodSounds mAres{"Are", "ares"};
    eGodSounds mArtemis{"Atm", "artemis"};
    eGodSounds mAthena{"Atn", "athena"};
    eGodSounds mAtlas{"Ats", "Atlas"};
    eGodSounds mDemeter{"Dem", "demeter"};
    eGodSounds mDionysus{"Dio", "dionysus"};
    eGodSounds mHades{"Had", "hades"};
    eGodSounds mHephaestus{"Hep", "hephaestus"};
    eGodSounds mHera{"Hra", "Hera"};
    eGodSounds mHermes{"Her", "hermes"};
    eGodSounds mPoseidon{"Pos", "poseidon"};
    eGodSounds mZeus{"Zeu", "zeus"};

    eHeroSounds mAchilles{"Ach", "achilles"};
    eHeroSounds mAtalanta{"Ata", "atalanta"};
    eHeroSounds mBellerophon{"Bel", "bellerophon"};
    eHeroSounds mHercules{"Hec", "hercules"};
    eHeroSounds mJason{"Jas", "jason"};
    eHeroSounds mOdysseus{"Ody", "odysseus"};
    eHeroSounds mPerseus{"Per", "perseus"};
    eHeroSounds mTheseus{"The", "theseus"};

    eMonsterSounds mCalydonianBoar{"cal", "calydonian"};
    eMonsterSounds mCerberus{"cer", "cerberus"};
    eMonsterSounds mChimera{"chi", "chimera"};
    eMonsterSounds mCyclops{"cyc", "cyclops"};
    eMonsterSounds mDragon{"dra", "dragon"};
    eMonsterSounds mEchidna{"ech", "echidna"};
    eMonsterSounds mHarpies{"har", "harpies"};
    eMonsterSounds mHector{"hec", "hector"};
    eMonsterSounds mHydra{"hyd", "hydra"};
    eMonsterSounds mKraken{"kra", "kraken"};
    eMonsterSounds mMaenads{"mae", "maenads"};
    eMonsterSounds mMedusa{"med", "medusa"};
    eMonsterSounds mMinotaur{"min", "minotaur"};
    eMonsterSounds mScylla{"scy", "scylla"};
    eMonsterSounds mSphinx{"sph", "sphinx"};
    eMonsterSounds mTalos{"tal", "talos"};
    eSatyrSounds mSatyr;

    eSoundVector mSettlerVoices;
    eSoundVector mHomelessVoices;
    eSoundVector mDisgruntledVoices;
    eSoundVector mSickVoices;

    eSoundVector mAphroditeVoices;
    eSoundVector mApolloVoices;
    eSoundVector mAresVoices;
    eSoundVector mArtemisVoices;
    eSoundVector mAthenaVoices;
    eSoundVector mAtlasVoices;
    eSoundVector mDemeterVoices;
    eSoundVector mDionysusVoices;
    eSoundVector mHadesVoices;
    eSoundVector mHephaestusVoices;
    eSoundVector mHeraVoices;
    eSoundVector mHermesVoices;
    eSoundVector mPoseidonVoices;
    eSoundVector mZeusVoices;

    eSoundVector mCalydonianBoarVoices;
    eSoundVector mCerberusVoices;
    eSoundVector mChimeraVoices;
    eSoundVector mCyclopsVoices;
    eSoundVector mDragonVoices;
    eSoundVector mEchidnaVoices;
    eSoundVector mHarpiesVoices;
    eSoundVector mHectorVoices;
    eSoundVector mHydraVoices;
    eSoundVector mKrakenVoices;
    eSoundVector mMaenadsVoices;
    eSoundVector mMedusaVoices;
    eSoundVector mMinotaurVoices;
    eSoundVector mScyllaVoices;
    eSoundVector mSphinxVoices;
    eSoundVector mTalosVoices;
    eSoundVector mSatyrVoices;

    eSoundVector mAchillesVoices;
    eSoundVector mAtalantaVoices;
    eSoundVector mBellerophonVoices;
    eSoundVector mHerculesVoices;
    eSoundVector mJasonVoices;
    eSoundVector mOdysseusVoices;
    eSoundVector mPerseusVoices;
    eSoundVector mTheseusVoices;

    eSoundVector mStoneMinerVoices;
    eSoundVector mActorVoices;
    eSoundVector mPhilosopherVoices;
    eSoundVector mArtisanVoices;
    eSoundVector mAstronomerVoices;
    eSoundVector mInventorVoices;
    eSoundVector mAthleteVoices;
    eSoundVector mCompetitorVoices;
    eSoundVector mBronzeMinerVoices;
    eSoundVector mSilverMinerVoices;
    eSoundVector mFireFighterVoices;
    eSoundVector mBoarHunterVoices;
    eSoundVector mDeerHunterVoices;
    eSoundVector mClerkVoices;
    eSoundVector mCuratorVoices;
    eSoundVector mFishingBoatVoices;
    eSoundVector mGrowerVoices;
    eSoundVector mOrangeTenderVoices;
    eSoundVector mGoatherdVoices;
    eSoundVector mUrchinGathererVoices;
    eSoundVector mShepherdVoices;
    eSoundVector mHealerVoices;
    eSoundVector mLumberjackVoices;
    eSoundVector mEliteVoices;
    eSoundVector mPeddlerVoices;
    eSoundVector mPriestVoices;
    eSoundVector mButcherVoices;
    eSoundVector mScholarVoices;
    eSoundVector mTraderVoices;
    eSoundVector mTradeBoatVoices;
    eSoundVector mTriremeVoices;
    eSoundVector mWatchmanVoices;
    eSoundVector mWaterDistributorVoices;

    eSoundVector mFoodVendorVoices;
    eSoundVector mFleeceVendorVoices;
    eSoundVector mOilVendorVoices;
    eSoundVector mWineVendorVoices;
    eSoundVector mArmorVendorVoices;
    eSoundVector mHorseVendorVoices;
    eSoundVector mChariotVendorVoices;
    eSoundVector mCartTransporterVoices;

    eSoundVector mChariotVoices;
    eSoundVector mHopliteVoices;
    eSoundVector mHorsemanVoices;
    eSoundVector mRockthrowerVoices;
    eSoundVector mAmazonVoices;
};

#endif // SOUNDS_H
