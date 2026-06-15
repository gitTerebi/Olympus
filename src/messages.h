#ifndef MESSAGES_H
#define MESSAGES_H

#include "message.h"

#include <map>

enum class GodType;
enum class eHeroType;
enum class eMonsterType;

struct QuestMessages {
    EventMessageType fQuest;
    EventMessageType fFulfilled;
};

struct GodMessages {
    mutable int fLastMessage = -1;

    MessageType fWooing0;
    MessageType fJealousy1;
    MessageType fJealousy2;
    MessageType fInvades;
    EventMessageType fHelps;

    EventMessageType fMonster;

    QuestMessages fQuest1;
    QuestMessages fQuest2;

    MessageType fSanctuaryComplete;

    MessageType fDisaster;
    MessageType fDisasterEnds;
};

struct MonsterMessages {
    MessageType fInCity;
    std::string fInCityReason;
    MessageType fPrediction;
    MessageType fInvasion36;
    MessageType fInvasion24;
    MessageType fInvasion12;
    MessageType fInvasion6;
    MessageType fInvasion1;
    MessageType fInvasion;
    MessageType fSlain;
    std::string fToSlainReason;
    std::string fSlainReason;
    std::string fMonsterAttackReason;
};

struct HeroMessages {
    EventMessageType fArrival;
    ConsequenceMessageType fHallAvailable;
};

struct RequestedResourcesMessages {
    MessageType fInitial;
    MessageType fReminder;
    MessageType fOverdue;
    MessageType fWarning;
    MessageType fComply;
    MessageType fTooLate;
    MessageType fRefuse;
    Reason fEarlyReason;
    Reason fComplyReason;
    Reason fTooLateReason;
    Reason fRefuseReason;
};

struct ResourceGrantedMessages {
    MessageType fGranted;
    MessageType fCashGranted;
    MessageType fPartialSpace;
    MessageType fInsufficientSpace;
    MessageType fLastChance;
    MessageType fForfeited;
    MessageType fAccepted;
    MessageType fCashAccepted;
    MessageType fPostponed;
    MessageType fRefused;
};

struct TroopsRequestedMessages {
    MessageType fInitialAnnouncement;
    MessageType fFirstReminder;
    MessageType fLastReminder;
    Reason fEarlyReason;
    Reason fComplyReason;
    Reason fTooLateReason;
    Reason fRefuseReason;
    Reason fLostBattleReason;

    MessageType fConquered;
};

struct TroopsRequestedMonsterMessages : public TroopsRequestedMessages {
};

struct TroopsRequestedAttackMessages : public TroopsRequestedMessages {
};

class Messages {
public:
    Messages() {}

    static Messages instance;

    static bool load();
    static bool loaded();

    static std::string message(const std::string& key);
    static MessageType favorMessage(const MessageType& reason);
    static MessageType dfavorMessage(const MessageType& reason);
    static MessageType invasionMessage(const MessageType& baseMsg,
                                        const std::string& reason,
                                        const int months);

    std::map<std::string, std::string> fMessages;

    MessageType fPop100;
    MessageType fPop500;
    MessageType fPop1000; // comm
    MessageType fPop2000; // comm
    MessageType fPop3000;
    MessageType fPop5000; // comm
    MessageType fPop10000; // comm
    MessageType fPop15000; // comm
    MessageType fPop20000; // comm
    MessageType fPop25000; // comm

    MessageType fHappinessMonument;

    MessageType fFire;
    MessageType fCollapse;
    MessageType fPlagueStrikes;

    const GodMessages* godMessages(const GodType godType) const;

    GodMessages fAphrodite;
    GodMessages fApollo;
    GodMessages fAres;
    GodMessages fArtemis;
    GodMessages fAthena;
    GodMessages fAtlas;
    GodMessages fDemeter;
    GodMessages fDionysus;
    GodMessages fHades;
    GodMessages fHephaestus;
    GodMessages fHera;
    GodMessages fHermes;
    GodMessages fPoseidon;
    GodMessages fZeus;

    MessageType fZeusTradeResumes;
    MessageType fPoseidonTradeResumes;
    MessageType fHermesTradeResumes;

    const HeroMessages* heroMessages(const eHeroType heroType) const;

    HeroMessages fAchilles;
    HeroMessages fAtalanta;
    HeroMessages fBellerophon;
    HeroMessages fHercules;
    HeroMessages fJason;
    HeroMessages fOdysseus;
    HeroMessages fPerseus;
    HeroMessages fTheseus;

    const MonsterMessages* monsterMessages(const eMonsterType monsterType) const;

    MonsterMessages fCalydonianBoar;
    MonsterMessages fCerberus;
    MonsterMessages fChimera;
    MonsterMessages fCyclops;
    MonsterMessages fDragon;
    MonsterMessages fEchidna;
    MonsterMessages fHarpies;
    MonsterMessages fHector;
    MonsterMessages fHydra;
    MonsterMessages fKraken;
    MonsterMessages fMaenads;
    MonsterMessages fMedusa;
    MonsterMessages fMinotaur;
    MonsterMessages fScylla;
    MonsterMessages fSphinx;
    MonsterMessages fTalos;

    GameMessages fHomeGames;
    GameMessages fIsthmianGames;
    GameMessages fNemeanGames;
    GameMessages fPythianGames;
    GameMessages fOlympianGames;

    MessageType fInvasionInitial;
    MessageType fInvasion24;
    MessageType fInvasion12;
    MessageType fInvasion6;
    MessageType fInvasion1;
    MessageType fInvasion;
    std::string fInvasionNoReason;
    MessageType fInvasionBribed;
    MessageType fInvasionRaidOver;
    MessageType fInvasionDefeat;
    MessageType fInvasionVictory;
    MessageType fInvasionVictoryMonument;
    MessageType fInvasionBribedMonument;

    MessageType fTributePaidGranted;
    MessageType fTributePaidAccepted;
    MessageType fTributePaidRefused;
    MessageType fTributePostponed;

    MessageType fRequestGranted;
    MessageType fRequestPartialSpace;
    MessageType fRequestInsufficientSpace;
    MessageType fRequestLastChance;
    MessageType fRequestAccepted;
    MessageType fRequestPostponed;
    MessageType fRequestRefused;
    MessageType fRequestForfeited;

    MessageType fDemandRefused;

    MessageType fGiftReceivedDrachmas;
    MessageType fGiftReceivedNeeded;
    MessageType fGiftReceivedSells;
    MessageType fGiftReceivedNotNeeded;
    MessageType fGiftReceivedRefuse;

    RequestedResourcesMessages fGeneralRequestAllyS;
    RequestedResourcesMessages fGeneralRequestRivalD;
    RequestedResourcesMessages fGeneralRequestSubjectP;
    RequestedResourcesMessages fGeneralRequestParentR;

    RequestedResourcesMessages fFamineAllyS;
    RequestedResourcesMessages fFamineRivalD;
    RequestedResourcesMessages fFamineSubjectP;
    RequestedResourcesMessages fFamineParentR;

    RequestedResourcesMessages fProjectAllyS;
    RequestedResourcesMessages fProjectRivalD;
    RequestedResourcesMessages fProjectSubjectP;
    RequestedResourcesMessages fProjectParentR;

    RequestedResourcesMessages fFestivalAllyS;
    RequestedResourcesMessages fFestivalRivalD;
    RequestedResourcesMessages fFestivalSubjectP;
    RequestedResourcesMessages fFestivalParentR;

    RequestedResourcesMessages fFinancialWoesAllyS;
    RequestedResourcesMessages fFinancialWoesRivalD;
    RequestedResourcesMessages fFinancialWoesSubjectP;
    RequestedResourcesMessages fFinancialWoesParentR;

    RequestedResourcesMessages fTributeRequest;

    ResourceGrantedMessages fGiftGranted;

    MessageType fSupportTroopsArrive;
    MessageType fSupportTroopsDepart;
    MessageType fSupportTroopsDefeated;

    MessageType fStrikeDepart;
    MessageType fStrikeSuccessful;
    MessageType fStrikeUnsuccessul;

    MessageType fRivalArmyAway;

    MessageType fIncreasedFavor;
    MessageType fDecreasedFavor;

    MessageType fUnemployment;
    MessageType fEmployees;

    MessageType fPlague;

    MessageType fCityConqueredByPlayer;
    MessageType fCityConquerFailedByPlayer;

    ResourceGrantedMessages fRaidGranted;
    MessageType fCityRaidFailedByPlayer;

    MessageType fAllyAttackedByPlayer;

    MessageType fArmyReturns;

    MessageType fMilitaryBuildup;
    MessageType fMilitaryDecline;

    MessageType fEconomicProsperity;
    MessageType fEconomicDecline;

    TroopsRequestedMessages fAllyTroopsRequest;
    TroopsRequestedMessages fVassalTroopsRequest;
    TroopsRequestedMessages fColonyTroopsRequest;
    TroopsRequestedMessages fParentCityTroopsRequest;

    MessageType fTroopsRequestAttackAverted;

    TroopsRequestedMonsterMessages fAllyTroopsMonsterRequest;
    TroopsRequestedMonsterMessages fVassalTroopsMonsterRequest;
    TroopsRequestedMonsterMessages fColonyTroopsMonsterRequest;
    TroopsRequestedMonsterMessages fParentCityTroopsMonsterRequest;

    MessageType fTroopsMonsterRequestAttackAverted;

    TroopsRequestedAttackMessages fAllyTroopsAttackRequest;
    TroopsRequestedAttackMessages fVassalTroopsAttackRequest;

    MessageType fEarthqaukeNoReason;
    MessageType fEarthqaukeGod;
    MessageType fTidalWaveNoReason;
    MessageType fTidalWaveGod;
    MessageType fLavaFlowNoReason;
    MessageType fLavaFlowGod;
    MessageType fSinkLandNoReason;
    MessageType fSinkLandGod;
    MessageType fLandSlide;

    MessageType fModestPyramidComplete1;
    MessageType fPyramidComplete2;
    MessageType fGreatPyramidComplete3;
    MessageType fMajesticPyramidComplete4;

    MessageType fSmallMonumentToTheSkyComplete5;
    MessageType fMonumentToTheSkyComplete6;
    MessageType fGrandMonumentToTheSkyComplete7;

    MessageType fMinorShrineComplete8;
    MessageType fShrineComplete9;
    MessageType fMajorShrineComplete10;

    MessageType fPyramidOfThePantheonComplete11;
    MessageType fAltarOfOlympusComplete12;
    MessageType fTempleOfOlympusComplete13;
    MessageType fObservatoryKosmikaComplete14;
    MessageType fMuseumAtlantikaComplete15;

    MessageType fCityBecomesAlly;
    MessageType fCityBecomesRival;
    MessageType fCityBecomesVassal;

    MessageType fCityBecomesActive;
    MessageType fCityBecomesInactive;

    MessageType fCityBecomesVisible;
    MessageType fCityBecomesInvisible;

    MessageType fTributeSuspended;
    MessageType fTributeResumed;

    MessageType fCityRebellion;
    MessageType fCityRebellionQuelled;
    MessageType fCityRebellionOver;
    MessageType fAllyResentful;
    MessageType fRivalHostile;
    MessageType fVassalRebellious;
    MessageType fColonyRebellious;
    MessageType fParentResentful;
    MessageType fAllyLovesYou;
    MessageType fRivalPleased;
    MessageType fVassalLovesYou;
    MessageType fColonyLovesYou;
    MessageType fParentLovesYou;

    MessageType fRivalConqueredByAlly;
    MessageType fRivalConqueredByVassal;

    MessageType fAllyConqueredByRival;
    MessageType fVassalConqueredByRival;
    MessageType fColonyConqueredByRival;
    MessageType fParentConqueredByRival;

    MessageType fColonyRestored;

    MessageType fDebtAnniversary;

    MessageType fColonyMonument;

    MessageType fPriceIncrease;
    MessageType fPriceDecrease;

    MessageType fDemandIncrease;
    MessageType fDemandDecrease;

    MessageType fSupplyIncrease;
    MessageType fSupplyDecrease;

    MessageType fTradeShutsDown;
    MessageType fTradeOpensUp;

    MessageType fWageIncrease;
    MessageType fWageDecrease;

    MessageType fAreaCutOff;
private:
    bool loadImpl();

    std::string loadMessage(const std::string& key) const;
    void loadMessage(MessageType& m,
                     const std::string& titleKey,
                     const std::string& textKey);
    void loadReason(Reason& r, const std::string& key);

    void load(GodMessages& god, const std::string& godName);
    void load(HeroMessages& hero, const std::string& heroName);
    void load(MonsterMessages& monster,
              const std::string& monsterName1,
              const std::string& monsterName2);

    void loadGeneralRequest(RequestedResourcesMessages& request,
                            const std::string& letter,
                            const std::string& mid = "general_request");
    void loadResourceGranted(ResourceGrantedMessages& granted,
                             const std::string& name);
    void load(TroopsRequestedMessages& troops,
              const std::string& name,
              const std::string& letter);
    void load(TroopsRequestedMonsterMessages& troops,
              const std::string& name);
    void load(TroopsRequestedAttackMessages& troops,
              const std::string& name,
              std::string letter);

    bool mLoaded = false;
};

#endif // MESSAGES_H
