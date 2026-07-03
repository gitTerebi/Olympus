#include "character-textures.h"

#include "sprite-loader.h"

#include "spriteData/amazonSpear30.h"

#include "spriteData/amazonArcher30.h"

#include "spriteData/aresWarrior30.h"

#include "spriteData/disgruntled30.h"

#include "spriteData/sick30.h"

#include "spriteData/homeless30.h"

#include "spriteData/trojanHoplite30.h"

#include "spriteData/trojanSpearthrower30.h"

#include "spriteData/trojanHorseman30.h"

#include "spriteData/centaurHorseman30.h"

#include "spriteData/centaurArcher30.h"

#include "spriteData/persianHoplite30.h"

#include "spriteData/persianHorseman30.h"

#include "spriteData/persianArcher30.h"

#include "offsets/Zeus_amazon.h"
#include "offsets/Zeus_AresWarriors.h"
#include "offsets/SprMain.h"
#include "offsets/Zeus_trojan.h"
#include "offsets/Zeus_centaur.h"
#include "offsets/Zeus_persian.h"

void CharacterTextures::loadDisgruntled() {
    if(fDisgruntledLoaded) return;
    fDisgruntledLoaded = true;
    const auto& sds = eDisgruntledSpriteData30;
    SpriteLoader loader(fTileH, "disgruntled", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(6033, 6033, 6129, fDisgruntled.fWalk);
    loader.loadSkipFlipped(6033, 6129, 6201, fDisgruntled.fFight);

    for(int i = 6129; i < 6137; i++) {
        loader.load(6033, i, fDisgruntled.fDie);
    }
}

void CharacterTextures::loadSick() {
    if(fSickLoaded) return;
    fSickLoaded = true;
    const auto& sds = eSickSpriteData30;
    SpriteLoader loader(fTileH, "sick", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(12232, 12232, 12328, fSick.fWalk);
    loader.loadSkipFlipped(12232, 12336, 12432, fSick.fFight);

    for(int i = 12328; i < 12336; i++) {
        loader.load(12232, i, fSick.fDie);
    }
}

void CharacterTextures::loadHomeless() {
    if(fHomelessLoaded) return;
    fHomelessLoaded = true;
    const auto& sds = eHomelessSpriteData30;
    SpriteLoader loader(fTileH, "homeless", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(1441, 1441, 1537, fHomeless.fWalk);

    for(int i = 1537; i < 1545; i++) {
        loader.load(1441, i, fHomeless.fDie);
    }
}

void CharacterTextures::loadPersianHoplite() {
    if(fPersianHopliteLoaded) return;
    fPersianHopliteLoaded = true;
    const auto& sds = ePersianHopliteSpriteData30;
    SpriteLoader loader(fTileH, "persianHoplite", sds,
                         &eZeus_persianOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 121, fPersianHoplite.fWalk);
    loader.loadSkipFlipped(1, 121, 241, fPersianHoplite.fFight);

    for(int i = 241; i < 257; i++) {
        loader.load(1, i, fPersianHoplite.fDie);
    }
}

void CharacterTextures::loadPersianArcher() {
    if(fPersianArcherLoaded) return;
    fPersianArcherLoaded = true;
    const auto& sds = ePersianArcherSpriteData30;
    SpriteLoader loader(fTileH, "persianArcher", sds,
                         &eZeus_persianOffset, fRenderer);

    loader.loadSkipFlipped(257, 257, 377, fPersianArcher.fWalk);
    loader.loadSkipFlipped(257, 377, 521, fPersianArcher.fFight);

    for(int i = 521; i < 532; i++) {
        loader.load(257, i, fPersianArcher.fDie);
    }
}

void CharacterTextures::loadPersianHorseman() {
    if(fPersianHorsemanLoaded) return;
    fPersianHorsemanLoaded = true;
    const auto& sds = ePersianHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "persianHorseman", sds,
                         &eZeus_persianOffset, fRenderer);

    loader.loadSkipFlipped(532, 532, 652, fPersianHorseman.fWalk);
    loader.loadSkipFlipped(532, 652, 772, fPersianHorseman.fFight);

    for(int i = 772; i < 787; i++) {
        loader.load(532, i, fPersianHorseman.fDie);
    }
}

void CharacterTextures::loadTrojanHoplite() {
    if(fTrojanHopliteLoaded) return;
    fTrojanHopliteLoaded = true;
    const auto& sds = eTrojanHopliteSpriteData30;
    SpriteLoader loader(fTileH, "trojanHoplite", sds,
                         &eZeus_trojanOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fTrojanHoplite.fWalk);
    loader.loadSkipFlipped(1, 105, 185, fTrojanHoplite.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fTrojanHoplite.fDie);
    }
}

void CharacterTextures::loadTrojanSpearthrower() {
    if(fTrojanSpearthrowerLoaded) return;
    fTrojanSpearthrowerLoaded = true;
    const auto& sds = eTrojanSpearthrowerSpriteData30;
    SpriteLoader loader(fTileH, "trojanSpearthrower", sds,
                         &eZeus_trojanOffset, fRenderer);

    loader.loadSkipFlipped(185, 185, 281, fTrojanSpearthrower.fWalk);
    loader.loadSkipFlipped(185, 289, 369, fTrojanSpearthrower.fFight);

    for(int i = 281; i < 289; i++) {
        loader.load(185, i, fTrojanSpearthrower.fDie);
    }
}

void CharacterTextures::loadTrojanHorseman() {
    if(fTrojanHorsemanLoaded) return;
    fTrojanHorsemanLoaded = true;
    const auto& sds = eTrojanHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "trojanHorseman", sds,
                         &eZeus_trojanOffset, fRenderer);

    loader.loadSkipFlipped(369, 369, 465, fTrojanHorseman.fWalk);
    loader.loadSkipFlipped(369, 473, 569, fTrojanHorseman.fFight);

    for(int i = 465; i < 473; i++) {
        loader.load(369, i, fTrojanHorseman.fDie);
    }
}

void CharacterTextures::loadCentaurHorseman() {
    if(fCentaurHorsemanLoaded) return;
    fCentaurHorsemanLoaded = true;
    const auto& sds = eCentaurHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "centaurHorseman", sds,
                         &eZeus_centaurOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 153, fCentaurHorseman.fWalk);
    loader.loadSkipFlipped(1, 153, 265, fCentaurHorseman.fFight);

    for(int i = 265; i < 280; i++) {
        loader.load(1, i, fCentaurHorseman.fDie);
    }
}

void CharacterTextures::loadCentaurArcher() {
    if(fCentaurArcherLoaded) return;
    fCentaurArcherLoaded = true;
    const auto& sds = eCentaurArcherSpriteData30;
    SpriteLoader loader(fTileH, "centaurArcher", sds,
                         &eZeus_centaurOffset, fRenderer);

    loader.loadSkipFlipped(280, 280, 416, fCentaurArcher.fWalk);
    loader.loadSkipFlipped(280, 416, 536, fCentaurArcher.fFight);

    for(int i = 536; i < 551; i++) {
        loader.load(280, i, fCentaurArcher.fDie);
    }
}

void CharacterTextures::loadAmazonSpear() {
    if(fAmazonSpearLoaded) return;
    fAmazonSpearLoaded = true;
    const auto& sds = eAmazonSpearSpriteData30;
    SpriteLoader loader(fTileH, "amazonSpear", sds,
                         &eZeus_amazonOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 121, fAmazonSpear.fWalk);
    loader.loadSkipFlipped(1, 121, 241, fAmazonSpear.fFight);

    for(int i = 241; i < 261; i++) {
        loader.load(1, i, fAmazonSpear.fDie);
    }
}

void CharacterTextures::loadAmazonArcher() {
    if(fAmazonArcherLoaded) return;
    fAmazonArcherLoaded = true;
    const auto& sds = eAmazonArcherSpriteData30;
    SpriteLoader loader(fTileH, "amazonArcher", sds,
                         &eZeus_amazonOffset, fRenderer);

    loader.loadSkipFlipped(261, 261, 381, fAmazonArcher.fWalk);
    loader.loadSkipFlipped(261, 381, 525, fAmazonArcher.fFight);

    for(int i = 525; i < 540; i++) {
        loader.load(261, i, fAmazonArcher.fDie);
    }
}

void CharacterTextures::loadAresWarrior() {
    if(fAresWarriorLoaded) return;
    fAresWarriorLoaded = true;
    const auto& sds = eAresWarriorSpriteData30;
    SpriteLoader loader(fTileH, "aresWarrior", sds,
                         &eZeus_AresWarriorsOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 89, fAresWarrior.fWalk);
    loader.loadSkipFlipped(1, 89, 241, fAresWarrior.fDie);
    loader.loadSkipFlipped(1, 241, 401, fAresWarrior.fFight);
}
