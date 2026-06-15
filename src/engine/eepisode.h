#ifndef EEPISODE_H
#define EEPISODE_H

#include "game-board.h"
#include "fileIO/save-archive.h"

class SaveArchive;

enum class eEpisodeType {
    parentCity,
    colony
};

struct eEpisode {
    virtual ~eEpisode() = default;
    virtual void serialize(SaveArchive& ar);

    bool availableBuilding(const eBuildingType type,
                           const int id = -1) const;

    void clear();

    GameBoard* fBoard = nullptr;
    WorldBoard* fWorldBoard = nullptr;

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

    std::map<eCityId, std::vector<GodType>> fFriendlyGods;

    std::map<eCityId, std::vector<stdsptr<eGameEvent>>> fEvents;
    std::vector<stdsptr<eEpisodeGoal>> fGoals;

    std::map<eCityId, eAvailableBuildings> fAvailableBuildings;

    std::map<eCityId, int> fMaxSanctuaries;
};

struct eParentCityEpisode : public eEpisode {
    void serialize(SaveArchive& ar) override {
        eEpisode::serialize(ar);
        ar.field("nextEpisode", fNextEpisode, eEpisodeType::parentCity);
    }

    eEpisodeType fNextEpisode{eEpisodeType::parentCity};
};

struct eColonyEpisode : public eEpisode {
    void serialize(SaveArchive& ar) override {
        eEpisode::serialize(ar);
        ar.worldCityField("city", fWorldBoard, fCity);
    }

    std::string fSelection;

    stdsptr<WorldCity> fCity;
};

#endif // EEPISODE_H
