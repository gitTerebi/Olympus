#ifndef EEPISODE_H
#define EEPISODE_H

#include "egameboard.h"

enum class eEpisodeType {
    parentCity,
    colony
};

struct eEpisode {
    virtual void read(eReadStream& src);
    virtual void write(eWriteStream& dst) const;

    bool availableBuilding(const eBuildingType type,
                           const int id = -1) const;

    void clear();

    eGameBoard* fBoard = nullptr;
    eWorldBoard* fWorldBoard = nullptr;

    int fDrachmasDelete = 2500;
    std::map<ePlayerId, int> fDrachmas;
    eDate fStartDate = eDate(1, eMonth::january, -1500);
    double fWageMultiplier = 1.;
    std::map<eResourceType, int> fPrices;

    std::string fTitle;
    std::string fIntroduction;
    std::string fComplete;

    std::vector<eGodType> fFriendlyGodsDelete;
    std::map<eCityId, std::vector<eGodType>> fFriendlyGods;

    std::vector<stdsptr<eGameEvent>> fEvents;
    std::vector<stdsptr<eEpisodeGoal>> fGoals;

    std::map<eCityId, eAvailableBuildings> fAvailableBuildings;
};

struct eParentCityEpisode : public eEpisode {
    void read(eReadStream& src) override {
        eEpisode::read(src);
        src >> fNextEpisode;
    }

    void write(eWriteStream& dst) const override {
        eEpisode::write(dst);
        dst << fNextEpisode;
    }

    eEpisodeType fNextEpisode{eEpisodeType::parentCity};
};

struct eColonyEpisode : public eEpisode {
    void read(eReadStream& src) override {
        eEpisode::read(src);
        src.readCity(fWorldBoard, [this](const stdsptr<eWorldCity>& c) {
            fCity = c;
        });
    }

    void write(eWriteStream& dst) const override {
        eEpisode::write(dst);
        dst.writeCity(fCity.get());
    }

    std::string fSelection;

    stdsptr<eWorldCity> fCity;
};

#endif // EEPISODE_H
