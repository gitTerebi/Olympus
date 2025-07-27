#ifndef ECAMPAIGN_H
#define ECAMPAIGN_H

#include "eepisode.h"

#include "eepisodegoal.h"

struct eSetAside {
    eResourceType fRes;
    int fCount;
    stdsptr<eWorldCity> fFrom;

    void read(eReadStream& src, eWorldBoard* const board) {
        src >> fRes;
        src >> fCount;
        src.readCity(board, [this](const stdsptr<eWorldCity>& city) {
            fFrom = city;
        });
    }

    void write(eWriteStream& dst) const {
        dst << fRes;
        dst << fCount;
        dst.writeCity(fFrom.get());
    }
};

struct eCampaignGlossary {
    std::string fFolderName;
    int fBitmap = 0;
    std::string fTitle;
    std::string fIntroduction;
    std::string fComplete;
};

class eCampaign {
public:
    eCampaign();

    void initialize(const std::string& name);

    int initialFunds(const ePlayerId pid) const;
    void setInitialFunds(const ePlayerId pid, const int f);

    const eDate& date() const { return mDate; }
    void setDate(const eDate& d) { mDate = d; }

    void setBitmap(const int b) { mBitmap = b; }

    using eMap = std::map<std::string, std::string>;
    static bool sLoadStrings(const std::string& path, eMap& map);
    bool loadStrings();
    bool writeStrings(const std::string& path) const;

    eWorldBoard& worldBoard() { return mWorldBoard; }
    eGameBoard& parentCityBoard()
    { return *mParentBoard; }
    eGameBoard& colonyBoard(const int id)
    { return *mColonyBoards[id]; }

    static bool sReadGlossary(const std::string& name,
                              eCampaignGlossary& glossary);
    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    void readPak(const std::string& path);

    bool load(const std::string& name);
    bool save() const;

    eEpisode* currentEpisode() const;
    void setCurrentColonyEpisode(const int e);
    void startEpisode();
    void episodeFinished();
    eEpisodeType currentEpisodeType() const
    { return mCurrentEpisodeType; }
    bool finished() const;
    std::vector<eColonyEpisode*> remainingColonies() const;

    std::vector<int> colonyEpisodesLeft() const;

    using eParentCityEpisodes = std::vector<stdsptr<eParentCityEpisode>>;
    eParentCityEpisodes& parentCityEpisodes()
    { return mParentCityEpisodes; }
    stdsptr<eParentCityEpisode> addParentCityEpisode();
    stdsptr<eParentCityEpisode> insertParentCityEpisode(const int id);
    void deleteParentCityEpisode(const int id);
    void setVictoryParentCityEpisode(const int id);
    void copyParentCityEpisodeSettings(const int from, const int to);

    using eColonyEpisodes = std::vector<stdsptr<eColonyEpisode>>;
    eColonyEpisodes& colonyEpisodes()
    { return mColonyEpisodes; }
    void copyColonyEpisodeSettings(const int from, const int to);
    void copyEpisodeSettings(eEpisode* const from, eEpisode* const to);

    const std::string& titleText() const { return mTitle; }
    const std::string& introductionText() const { return mIntroduction; }
    const std::string& completeText() const { return mComplete; }

    void setDifficulty(const eDifficulty d);
    eDifficulty difficulty() const { return mDifficulty; }

    void setEditorMode(const bool e);

    void setAside(const eResourceType res, const int count,
                  const stdsptr<eWorldCity>& from);

    std::map<eResourceType, int>& prices()
    { return mPrices; }

    stdsptr<eWorldCity> lastPlayedColony() const;

    bool colonyEpisodeFinished(const int id) const;
private:
    int mBitmap = 0;
    std::string mName;
    std::string mTitle;
    std::string mIntroduction;
    std::string mComplete;

    int mCurrentParentEpisode = 0;
    int mCurrentColonyEpisode = 0;
    eEpisodeType mCurrentEpisodeType = eEpisodeType::parentCity;
    eEpisodeType mPreviousEpisodeType = eEpisodeType::parentCity;

    int mDrachmasDelete = 5000;
    std::map<ePlayerId, int> mDrachmas;
    eDate mDate = eDate(1, eMonth::january, -1500);
    double mWageMultiplier = 1.;
    std::map<eResourceType, int> mPrices;

    eDifficulty mDifficulty{eDifficulty::hero};

    eWorldBoard mWorldBoard;
    stdsptr<eGameBoard> mParentBoard;
    std::vector<stdsptr<eGameBoard>> mColonyBoards;

    std::vector<stdsptr<eParentCityEpisode>> mParentCityEpisodes;
    std::vector<stdsptr<eColonyEpisode>> mColonyEpisodes;
    std::vector<int> mPlayedColonyEpisodes;

    std::vector<eSetAside> mForColony;
    std::vector<eSetAside> mForParent;
};

#endif // ECAMPAIGN_H
