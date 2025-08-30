#ifndef ESOUNDS_H
#define ESOUNDS_H

#include "esoundvector.h"

#include "egodsounds.h"
#include "eherosounds.h"
#include "emonstersounds.h"

class eTile;

class eSounds {
public:
    eSounds();

    static void loadButtonSound();

    static void load();
    static bool loaded();

    static void playButtonSound();

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
    static void playAttackSound(const eCharacterType type);
    static void playDieSound(eCharacter* const c);
    static void playHitSound(eCharacter* const c);
private:
    void loadImpl();

    static eSounds* sInstance;

    bool mLoaded{false};

    eSoundVector mButton;

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

    eSoundVector mTrojanHorseDie;
    eSoundVector mTrojanHorseHit;

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
};

#endif // ESOUNDS_H
