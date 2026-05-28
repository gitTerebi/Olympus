#ifndef EEPISODE_H
#define EEPISODE_H

#include "e-game-board.h"
#include "fileIO/esavearchive.h"

class eSaveArchive;

enum class eEpisodeType {
    parentCity,
    colony
};

struct eEpisode {
    virtual ~eEpisode() = default;
    virtual void serialize(eSaveArchive& ar);

    bool availableBuilding(const eBuildingType type,
                           const int id = -1) const;

    void clear();

    GameBoard* fBoard = nullptr;
    eWorldBoard* fWorldBoard = nullptr;

    int fDrachmasDelete = 2500;
    std::map<ePlayerId, int> fDrachmas;
    eDate fStartDate = eDate(1, eMonth::january, -1500);
    double fWageMultiplier = 1.;
    std::map<eResourceType, int> fPrices;

    uint16_t fIntroId = 0;
    uint16_t fCompleteId = 0;

    std::string fTitle;
    std::string fIntroduction;
    std::string fComplete;

    std::map<eCityId, std::vector<eGodType>> fFriendlyGods;

    std::map<eCityId, std::vector<stdsptr<eGameEvent>>> fEvents;
    std::vector<stdsptr<eEpisodeGoal>> fGoals;

    std::map<eCityId, eAvailableBuildings> fAvailableBuildings;

    std::map<eCityId, int> fMaxSanctuaries;
};

struct eParentCityEpisode : public eEpisode {
    void serialize(eSaveArchive& ar) override {
        eEpisode::serialize(ar);
        ar.field("nextEpisode", fNextEpisode, eEpisodeType::parentCity);
    }

    eEpisodeType fNextEpisode{eEpisodeType::parentCity};
};

struct eColonyEpisode : public eEpisode {
    void serialize(eSaveArchive& ar) override {
        eEpisode::serialize(ar);
        ar.worldCityField("city", fWorldBoard, fCity);
    }

    std::string fSelection;

    stdsptr<eWorldCity> fCity;
};

#endif // EEPISODE_H
