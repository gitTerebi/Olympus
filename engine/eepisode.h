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
    virtual void read(eReadStream& src);
    virtual void write(eWriteStream& dst) const;
    void serialize(eSaveArchive& ar);
    virtual void serializeJson(class eJsonArchive& ar);

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
    void read(eReadStream& src) override {
        eEpisode::read(src);
        eSaveArchive ar(src);
        ar.field("nextEpisode", fNextEpisode);
    }

    void write(eWriteStream& dst) const override {
        eEpisode::write(dst);
        eSaveArchive ar(dst);
        ar.field("nextEpisode", const_cast<eEpisodeType&>(fNextEpisode));
    }

    void serializeJson(class eJsonArchive& ar) override;

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

    void serializeJson(class eJsonArchive& ar) override;

    std::string fSelection;

    stdsptr<eWorldCity> fCity;
};

#endif // EEPISODE_H
