#ifndef EMUSIC_H
#define EMUSIC_H

#include "emusicvector.h"

#include <memory>
#include <map>

enum class eMusicType {
    none, setup, music, battle
};

class eMusic {
public:
    eMusic();

    static void loadMenu();
    static void load();
    static bool loaded();

    static void incTime();

    static void playMenuMusic();
    static void playRandomMusic();
    static void playRandomBattleMusic();
    static void playMissionIntroMusic();
    static void playMissionVictoryMusic();
    static void playCampaignVictoryMusic();

    static bool playCampaignVoice(const std::string& path);
private:
    void incTimeImpl();

    void playMenuMusicImpl();
    void playRandomMusicImpl();
    void playRandomBattleMusicImpl();
    void playMissionIntroMusicImpl();
    void playMissionVictoryMusicImpl();
    void playCampaignVictoryMusicImpl();
    bool playCampaignVoiceImpl(const std::string& path);

    void loadImpl();
    void loadMenuImpl();
    static eMusic* sInstance;

    bool mLoaded{false};
    bool mMenuLoaded{false};
    eMusicType mMusicType{eMusicType::none};

    std::map<std::string, std::shared_ptr<eMusicVector>> mCampaignVoice;

    eMusicVector mSetupMusic;
    eMusicVector mMusic;
    eMusicVector mBattleMusic;
    eMusicVector mMissionIntro;
    eMusicVector mMissionVictory;
    eMusicVector mCampaignVictory;
};

#endif // EMUSIC_H
