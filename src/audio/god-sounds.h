#ifndef GOD_SOUNDS_H
#define GOD_SOUNDS_H

#include "sound-vector.h"

#include "characters/gods/god.h"

enum class eGodSound {
    wooing0,
    jealousy1,
    jealousy2,
    invade,
    monster,
    quest,
    questFinished,
    help,
    appear,
    disappear,
    attack,
    curse,
    hit,
    santcify
};

struct eGodSounds {
    eGodSounds(const std::string& shortName,
               const std::string& longName);

    void load();

    void play(const eGodSound s);

    void playWooing0() {
        fWooing0->playRandomSound(eSoundType::voice);
    }

    void playJealousy1() {
        fJealousy1->playRandomSound(eSoundType::voice);
    }

    void playJealousy2() {
        fJealousy2->playRandomSound(eSoundType::voice);
    }

    void playInvade() {
        fInvade->playRandomSound(eSoundType::voice);
    }

    void playMonster() {
        fMonster->playRandomSound(eSoundType::voice);
    }

    void playQuest() {
        fQuest->playRandomSound(eSoundType::voice);
    }

    void playQuestFinished() {
        fQuestFinished->playRandomSound(eSoundType::voice);
    }

    void playHelp() {
        fHelp->playRandomSound(eSoundType::voice);
    }

    void playAppear() {
        fAppear->playRandomSound();
    }

    void playDisappear() {
        fDisappear->playRandomSound();
    }

    void playAttack() {
        fAttack->playRandomSound();
    }

    void playCurse() {
        fCurse->playRandomSound();
    }

    void playHit() {
        fHit->playRandomSound();
    }

    void playSanctify() {
        fSanctify->playRandomSound();
    }

    const std::string fShortName;
    const std::string fLongName;

    bool fFirstVisit = true;

    std::shared_ptr<eSoundVector> fWooing0 = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fJealousy1 = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fJealousy2 = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fInvade = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fMonster = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fQuest = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fQuestFinished = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fHelp = std::make_shared<eSoundVector>();

    std::shared_ptr<eSoundVector> fAppear = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fDisappear = std::make_shared<eSoundVector>();

    std::shared_ptr<eSoundVector> fAttack = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fCurse = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fHit = std::make_shared<eSoundVector>();
    std::shared_ptr<eSoundVector> fSanctify = std::make_shared<eSoundVector>();
private:
    bool mLoaded = false;
};

#endif // GOD_SOUNDS_H
