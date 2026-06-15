#ifndef INTERFACE_TEXTURES_H
#define INTERFACE_TEXTURES_H

#include "texture-collection.h"

class InterfaceTextures {
public:
    InterfaceTextures(const int tileW, const int tileH,
                       SDL_Renderer* const renderer);

    void loadAll();

    void load();
    bool fLoaded = false;

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    void loadMapBinary(bool& loaded,
                       const std::string& name,
                       std::shared_ptr<Texture>& tex) const;
    std::string loadMapDirPath() const;

    bool fMapOfGreece1Loaded = false;
    void loadMapOfGreece1();
    bool fMapOfGreece2Loaded = false;
    void loadMapOfGreece2();
    bool fMapOfGreece3Loaded = false;
    void loadMapOfGreece3();
    bool fMapOfGreece4Loaded = false;
    void loadMapOfGreece4();
    bool fMapOfGreece5Loaded = false;
    void loadMapOfGreece5();
    bool fMapOfGreece6Loaded = false;
    void loadMapOfGreece6();
    bool fMapOfGreece7Loaded = false;
    void loadMapOfGreece7();
    bool fMapOfGreece8Loaded = false;
    void loadMapOfGreece8();

    bool fPoseidonMap1Loaded = false;
    void loadPoseidonMap1();
    bool fPoseidonMap2Loaded = false;
    void loadPoseidonMap2();
    bool fPoseidonMap3Loaded = false;
    void loadPoseidonMap3();
    bool fPoseidonMap4Loaded = false;
    void loadPoseidonMap4();

    bool fPoseidonCampaign1Loaded = false;
    void loadPoseidonCampaign1();
    bool fPoseidonCampaign2Loaded = false;
    void loadPoseidonCampaign2();
    bool fPoseidonCampaign3Loaded = false;
    void loadPoseidonCampaign3();
    bool fPoseidonCampaign4Loaded = false;
    void loadPoseidonCampaign4();
    bool fPoseidonCampaign5Loaded = false;
    void loadPoseidonCampaign5();
    bool fPoseidonCampaign6Loaded = false;
    void loadPoseidonCampaign6();

    std::shared_ptr<Texture> fGameTopBar;
    std::shared_ptr<Texture> fGameMenuBackground;
    std::shared_ptr<Texture> fGameArmyBackground;
    std::shared_ptr<Texture> fArmyStatus;
    TextureCollection fGoToCompany;
    TextureCollection fRotateCompany;
    TextureCollection fGoToBanner;
    TextureCollection fGoHome;
    TextureCollection fDefensiveTactics;
    TextureCollection fOffensiveTactics;
    TextureCollection fSpecialTactics;

    // normal, hover, selected, disabled
    TextureCollection fShowInfo;
    TextureCollection fShowMap;

    // normal, hover, selected, disabled
    TextureCollection fPopulation;
    TextureCollection fHusbandry;
    TextureCollection fIndustry;
    TextureCollection fDistribution;
    TextureCollection fHygieneSafety;
    TextureCollection fAdministration;
    TextureCollection fCulture;
    TextureCollection fScience; // poseidon
    TextureCollection fMythology;
    TextureCollection fMilitary;
    TextureCollection fAesthetics;
    TextureCollection fOverview;

    std::shared_ptr<Texture> fWorldMenuBackground;

    TextureCollection fWorldSmallButton;
    TextureCollection fWorldBigButton;

    TextureCollection fRequestButton;
    TextureCollection fFulfillButton;
    TextureCollection fGiftButton;
    TextureCollection fRaidButton;
    TextureCollection fConquerButton;

    TextureCollection fWorldLeftArrowButton;
    TextureCollection fWorldRightArrowButton;
    TextureCollection fWorldHistoryButton;

    TextureCollection fWorldBgsButton;

    std::shared_ptr<Texture> fMapEditMenuBackground;

    TextureCollection fBrushSize;
    TextureCollection fEmptyLand;
    TextureCollection fForest;
    TextureCollection fWaterMarshBeach;
    TextureCollection fMeadow;
    TextureCollection fFishAndUrchin;
    TextureCollection fRocks;
    TextureCollection fScrub;
    TextureCollection fElevation;
    TextureCollection fDisasters;
    TextureCollection fWaterPoints;
    TextureCollection fLandInvasionPoints;
    TextureCollection fExitEndEntryPoints;
    TextureCollection fAnimalPoints;

    // normal, hover, pressed
    TextureCollection fCommonHousing;
    TextureCollection fEliteHousing;

    TextureCollection fPoseidonCommonHousing;
    TextureCollection fPoseidonEliteHousing;

    TextureCollection fFoodFarming;
    TextureCollection fOtherFarming; // e.g. wine
    TextureCollection fAnimalFarming;
    TextureCollection fAnimalHunting;

    TextureCollection fResources; // e.g. wood, marble
    TextureCollection fProcessing; // e.g. olives
    TextureCollection fArtisansGuild;

    TextureCollection fGranary;
    TextureCollection fWarehouse;
    TextureCollection fAgoras;
    TextureCollection fTrade;

    TextureCollection fFireFighter;
    TextureCollection fFountain;
    TextureCollection fPolice;
    TextureCollection fHospital;

    TextureCollection fPalace;
    TextureCollection fTaxCollector;
    TextureCollection fBridge;

    TextureCollection fPoseidonBridge;

    TextureCollection fPhilosophy;
    TextureCollection fGymnasium;
    TextureCollection fDrama;
    TextureCollection fStadium;

    TextureCollection fBibliotheke;
    TextureCollection fAstronomy;
    TextureCollection fTechnology;
    TextureCollection fMuseum;

    TextureCollection fTemples;
    TextureCollection fHeroShrines;

    TextureCollection fPoseidonTemples;
    TextureCollection fPoseidonHeroShrines;

    TextureCollection fPiramids;
    TextureCollection fHipodrome;

    TextureCollection fFortifications;
    TextureCollection fMilitaryProduction;

    TextureCollection fBeautification;
    TextureCollection fRecreation;
    TextureCollection fMonuments;

    // normal, hover, pressed, disabled
    TextureCollection fBuildRoad;
    TextureCollection fRoadBlock;
    TextureCollection fClear;
    TextureCollection fUndo;

    TextureCollection fMessages;
    TextureCollection fMoreInfo;

    TextureCollection fGoals;
    TextureCollection fRotation;
    TextureCollection fWorld;

    // normal, hoer, pressed
    TextureCollection fFireAlert;
    TextureCollection fGroundFissureAlert;
    TextureCollection fFloodAlert;
    TextureCollection fAvalancheAltert;
    TextureCollection fLavaAltert;
    TextureCollection fCollapseAltert;
    TextureCollection fRoadBlockAltert;
    TextureCollection fVirusAlert;
    TextureCollection fIllnessAlert;
    TextureCollection fInvasionAlert;
    TextureCollection fMonsterAltert;
    TextureCollection fGodAttackAlert;
    TextureCollection fGodVisitAlert;
    TextureCollection fHeroArrivalAlert;
    TextureCollection fArmyComebackAlert;

    // 4 left, 4 middle, 4 right
    // normal, hover, pressed, disabled
    std::vector<TextureCollection> fMicroButton;

    // normal, hover, pressed, disabled
    TextureCollection fMilitaryControlManual;
    TextureCollection fMilitaryControlAutomatic;

    std::shared_ptr<Texture> fSoldiersIcon;
    std::shared_ptr<Texture> fShipsIcon;
    std::shared_ptr<Texture> fTowersIcon;

    // topLeft, top, topRight, left, center, right, bottomLeft, bottom, bottomRight,
    std::vector<TextureCollection> fComboBox;
    std::vector<TextureCollection> fMessageBox;
    std::vector<TextureCollection> fInnerBox;

    TextureCollection fBuildingButton;
    TextureCollection fBuildingButtonHover;

    std::shared_ptr<Texture> fLoadImage1;
    std::shared_ptr<Texture> fLoadImage2;
    std::shared_ptr<Texture> fLoadImage3;
    std::shared_ptr<Texture> fLoadImage4;
    std::shared_ptr<Texture> fLoadImage5;
    std::shared_ptr<Texture> fLoadImage6;
    std::shared_ptr<Texture> fLoadImage7;
    std::shared_ptr<Texture> fLoadImage8;
    std::shared_ptr<Texture> fLoadImage9;
    std::shared_ptr<Texture> fLoadImage10;
    std::shared_ptr<Texture> fLoadImage11;
    std::shared_ptr<Texture> fLoadImage12;
    std::shared_ptr<Texture> fMainMenuImage;
    std::shared_ptr<Texture> fIntroductionImage;
    std::shared_ptr<Texture> fDefeatImage;

    std::shared_ptr<Texture> fPoseidonCampaign1;
    std::shared_ptr<Texture> fPoseidonCampaign2;
    std::shared_ptr<Texture> fPoseidonCampaign3;
    std::shared_ptr<Texture> fPoseidonCampaign4;
    std::shared_ptr<Texture> fPoseidonCampaign5;
    std::shared_ptr<Texture> fPoseidonCampaign6;

    TextureCollection fCheckBox;
    TextureCollection fLargeCheckBox;

    TextureCollection fRequestWaitingBox;
    TextureCollection fRequestFulfilledBox;

    TextureCollection fSeeButton;

    //

    std::shared_ptr<Texture> fDrachmasTopMenu;
    std::shared_ptr<Texture> fPopulationTopMenu;
    std::shared_ptr<Texture> fDrachmasUnit;

    std::shared_ptr<Texture> fZeusQuestIcon;
    std::shared_ptr<Texture> fPoseidonQuestIcon;
    std::shared_ptr<Texture> fDemeterQuestIcon;
    std::shared_ptr<Texture> fApolloQuestIcon;
    std::shared_ptr<Texture> fArtemisQuestIcon;
    std::shared_ptr<Texture> fAresQuestIcon;
    std::shared_ptr<Texture> fAphroditeQuestIcon;
    std::shared_ptr<Texture> fHermesQuestIcon;
    std::shared_ptr<Texture> fAthenaQuestIcon;
    std::shared_ptr<Texture> fHephaestusQuestIcon;
    std::shared_ptr<Texture> fDionysusQuestIcon;
    std::shared_ptr<Texture> fHadesQuestIcon;

    std::shared_ptr<Texture> fHeraQuestIcon;
    std::shared_ptr<Texture> fAtlasQuestIcon;

    std::shared_ptr<Texture> fTroopsRequestIcon;

    std::shared_ptr<Texture> fUrchinUnit;
    std::shared_ptr<Texture> fFishUnit;
    std::shared_ptr<Texture> fMeatUnit;
    std::shared_ptr<Texture> fCheeseUnit;
    std::shared_ptr<Texture> fCarrotsUnit;
    std::shared_ptr<Texture> fOnionsUnit;
    std::shared_ptr<Texture> fWheatUnit;
    std::shared_ptr<Texture> fOrangesUnit;
    std::shared_ptr<Texture> fBlackMarbleUnit;
    std::shared_ptr<Texture> fOrichalcUnit;

    std::shared_ptr<Texture> fWoodUnit;
    std::shared_ptr<Texture> fBronzeUnit;
    std::shared_ptr<Texture> fMarbleUnit;
    std::shared_ptr<Texture> fGrapesUnit;
    std::shared_ptr<Texture> fOlivesUnit;
    std::shared_ptr<Texture> fFleeceUnit;
    std::shared_ptr<Texture> fHorseUnit;
    std::shared_ptr<Texture> fChariotUnit;
    std::shared_ptr<Texture> fArmsUnit;
    std::shared_ptr<Texture> fSculptureUnit;
    std::shared_ptr<Texture> fOliveOilUnit;
    std::shared_ptr<Texture> fWineUnit;
    std::shared_ptr<Texture> fFoodUnit;

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

    // normal, pressed
    TextureCollection fSmallUpButton;
    TextureCollection fSmallDownButton;

    // normal, hover, pressed
    TextureCollection fSpeakButton;

    // normal, hover, pressed, disabled
    TextureCollection fUpButton;
    TextureCollection fDownButton;
    TextureCollection fLeftButton;
    TextureCollection fRightButton;

    TextureCollection fBigUpButton;
    TextureCollection fBigDownButton;

    TextureCollection fExclamationButton;
    TextureCollection fProceedButton;
    TextureCollection fOkButton;
    TextureCollection fHelpButton;

    TextureCollection fAcceptButton;
    TextureCollection fCancelButton;

    TextureCollection fButtonFrame;
    TextureCollection fButtonFrameHover;

    TextureCollection fDiamond;

    // world
    std::shared_ptr<Texture> fZeusMainCity;
    std::shared_ptr<Texture> fZeusCollony;
    std::shared_ptr<Texture> fZeusDisabledCollony;
    std::shared_ptr<Texture> fZeusGreekCity;
    std::shared_ptr<Texture> fZeusTrojanCity;
    std::shared_ptr<Texture> fZeusPersianCity;
    std::shared_ptr<Texture> fZeusCentaurCity;
    std::shared_ptr<Texture> fZeusAmazonCity;

    std::shared_ptr<Texture> fZeusPlace;
    std::shared_ptr<Texture> fZeusRuins;

    std::shared_ptr<Texture> fZeusDistantCity;
    std::shared_ptr<Texture> fZeusDistantCityN;
    std::shared_ptr<Texture> fZeusDistantCityNE;
    std::shared_ptr<Texture> fZeusDistantCityE;
    std::shared_ptr<Texture> fZeusDistantCitySE;
    std::shared_ptr<Texture> fZeusDistantCityS;
    std::shared_ptr<Texture> fZeusDistantCitySW;
    std::shared_ptr<Texture> fZeusDistantCityW;
    std::shared_ptr<Texture> fZeusDistantCityNW;

    std::shared_ptr<Texture> fPoseidonMainCity;
    std::shared_ptr<Texture> fPoseidonCollony;
    std::shared_ptr<Texture> fPoseidonDisabledCollony;
    std::shared_ptr<Texture> fPoseidonEgyptianCity;
    std::shared_ptr<Texture> fPoseidonMayanCity;
    std::shared_ptr<Texture> fPoseidonPhoenicianCity;
    std::shared_ptr<Texture> fPoseidonOceanidCity;
    std::shared_ptr<Texture> fPoseidonAtlanteanCity;

    std::shared_ptr<Texture> fZeusAchilles;
    std::shared_ptr<Texture> fZeusHercules;
    std::shared_ptr<Texture> fZeusJason;
    std::shared_ptr<Texture> fZeusOdysseus;
    std::shared_ptr<Texture> fZeusPerseus;
    std::shared_ptr<Texture> fZeusTheseus;

    std::shared_ptr<Texture> fPoseidonAtalanta;
    std::shared_ptr<Texture> fPoseidonBellerophon;

    TextureCollection fZeusPlayerArmy;
    TextureCollection fZeusGreekArmy;
    TextureCollection fZeusTrojanArmy;
    TextureCollection fZeusPersianArmy;
    TextureCollection fZeusCentaurArmy;
    TextureCollection fZeusAmazonArmy;

    TextureCollection fPoseidonPlayerArmy;
    TextureCollection fPoseidonEgyptianArmy;
    TextureCollection fPoseidonMayanArmy;
    TextureCollection fPoseidonPhoenicianArmy;
    TextureCollection fPoseidonOceanidArmy;
    TextureCollection fPoseidonAtlanteanArmy;

    std::shared_ptr<Texture> fMainCityFlag;
    std::shared_ptr<Texture> fEmpireCityFlag;
    TextureCollection fAllyCityFlag;

    TextureCollection fCityArmy;
    TextureCollection fCityWealth;
    TextureCollection fCityRebellion;

    std::shared_ptr<Texture> fPoseidonMap1;
    std::shared_ptr<Texture> fPoseidonMap2;
    std::shared_ptr<Texture> fPoseidonMap3;
    std::shared_ptr<Texture> fPoseidonMap4;

    std::shared_ptr<Texture> fMapOfGreece1;
    std::shared_ptr<Texture> fMapOfGreece2;
    std::shared_ptr<Texture> fMapOfGreece3;
    std::shared_ptr<Texture> fMapOfGreece4;
    std::shared_ptr<Texture> fMapOfGreece5;
    std::shared_ptr<Texture> fMapOfGreece6;
    std::shared_ptr<Texture> fMapOfGreece7;
    std::shared_ptr<Texture> fMapOfGreece8;

    TextureCollection fInterfaceBanners;
    TextureCollection fInterfaceBannerTops;
    TextureCollection fPoseidonInterfaceBannerTops;

    TextureCollection fZeusPortraits;
    TextureCollection fZeusGodPortraits;
    TextureCollection fPoseidonPortraits;
};

#endif // INTERFACE_TEXTURES_H
