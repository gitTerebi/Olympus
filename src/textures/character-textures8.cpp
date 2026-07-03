#include "character-textures.h"

#include "sprite-loader.h"

#include "spriteData/scholar30.h"

#include "spriteData/astronomer30.h"

#include "spriteData/inventor30.h"

#include "spriteData/curator30.h"

#include "spriteData/hoplitePoseidon30.h"

#include "spriteData/chariotPoseidon30.h"

#include "spriteData/archerPoseidon30.h"

#include "spriteData/cattle130.h"

#include "spriteData/cattle230.h"

#include "spriteData/cattle330.h"

#include "spriteData/bull30.h"

#include "spriteData/butcher30.h"

#include "spriteData/chariotVendorCharacter30.h"

#include "spriteData/chariot30.h"

#include "spriteData/elephant30.h"

#include "spriteData/poseidonTowerArcher30.h"

#include "spriteData/triremeOverlay30.h"

#include "spriteData/orichalcMiner30.h"

#include "spriteData/hippodromeHorse130.h"

#include "spriteData/hippodromeHorse230.h"

#include "spriteData/hippodromeHorse330.h"

#include "spriteData/hippodromeHorse430.h"

#include "offsets/PoseidonImps.h"

void CharacterTextures::loadOrichalcMiner() {
    if(fOrichalcMinerLoaded) return;
    fOrichalcMinerLoaded = true;
    const auto& sds = eOrichalcMinerSpriteData30;
    SpriteLoader loader(fTileH, "orichalcMiner", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(1507, 1507, 1603, fOrichalcMiner.fWalk);
    loader.loadSkipFlipped(1507, 1719, 1815, fOrichalcMiner.fCarry);
    loader.loadSkipFlipped(1507, 1611, 1707, fOrichalcMiner.fCollect);

    for(int i = 1603; i < 1611; i++) {
        loader.load(1507, i, fOrichalcMiner.fDie);
    }
}

void CharacterTextures::loadTriremeOverlay() {
    if(fTriremeOverlayLoaded) return;
    fTriremeOverlayLoaded = true;
    const auto& sds = eTriremeOverlaySpriteData30;
    SpriteLoader loader(fTileH, "triremeOverlay", sds,
                         &ePoseidonImpsOffset, fRenderer);

    for(int j = 0; j < 8; j++) {
        fTriremeOverlay.emplace_back(fRenderer);
    }
    loader.loadSkipFlipped(8300, 8300, 8332, fTriremeOverlay);

    for(int j = 0; j < 8; j++) {
        fTriremeDieOverlay.emplace_back(fRenderer);
    }
    loader.loadSkipFlipped(8300, 8332, 8428, fTriremeDieOverlay);

    for(int j = 0; j < 8; j++) {
        fTriremeFightOverlay.emplace_back(fRenderer);
    }
    loader.loadSkipFlipped(8300, 8428, 8524, fTriremeFightOverlay);
}

void CharacterTextures::loadPoseidonTowerArcher() {
    if(fPoseidonTowerArcherLoaded) return;
    fPoseidonTowerArcherLoaded = true;
    const auto& sds = ePoseidonTowerArcherSpriteData30;
    SpriteLoader loader(fTileH, "poseidonTowerArcher", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(4398, 4398, 4494, fPoseidonTowerArcher.fWalk);

    for(int i = 4494; i < 4502; i++) {
        loader.load(4398, i, fPoseidonTowerArcher.fDie);
    }

    loader.loadSkipFlipped(4398, 4502, 4622, fPoseidonTowerArcher.fFight);
    loader.loadSkipFlipped(4398, 4622, 4718, fPoseidonTowerArcher.fPatrol);
}

void CharacterTextures::loadElephant() {
    if(fElephantLoaded) return;
    fElephantLoaded = true;
    const auto& sds = eElephantSpriteData30;
    SpriteLoader loader(fTileH, "elephant", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(5078, 5078, 5174, fElephant.fWalk);

    for(int i = 5174; i < 5182; i++) {
        loader.load(5078, i, fElephant.fDie);
    }
}

void CharacterTextures::loadChariot() {
    if(fChariotLoaded) return;
    fChariotLoaded = true;
    const auto& sds = eChariotSpriteData30;
    SpriteLoader loader(fTileH, "chariot", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(7756, 7756, 7796, fChariot);
}

void CharacterTextures::loadChariotVendor() {
    if(fChariotVendorLoaded) return;
    fChariotVendorLoaded = true;
    const auto& sds = eChariotVendorCharacterSpriteData30;
    SpriteLoader loader(fTileH, "chariotVendorCharacter", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(7876, 7876, 7972, fChariotVendor.fWalk);

    for(int i = 7972; i < 7980; i++) {
        loader.load(7876, i, fChariotVendor.fDie);
    }
}

void sLoadCattle(SpriteLoader& loader,
                 CattleTextures& cattle,
                 const int first) {
    loader.loadSkipFlipped(first, first, first + 96, cattle.fWalk);
    loader.loadSkipFlipped(first, first + 96, first + 192, cattle.fStand);

    for(int i = first + 192; i < first + 200; i++) {
        loader.load(first, i, cattle.fDie);
    }
};

void CharacterTextures::loadCattle() {
    if(fCattleLoaded) return;
    fCattleLoaded = true;
    {
        const auto& sds = eCattle1SpriteData30;
        SpriteLoader loader(fTileH, "cattle1", sds,
                             &ePoseidonImpsOffset, fRenderer);

        sLoadCattle(loader, fCattle1, 2290);
    }
    {
        const auto& sds = eCattle2SpriteData30;
        SpriteLoader loader(fTileH, "cattle2", sds,
                             &ePoseidonImpsOffset, fRenderer);

        sLoadCattle(loader, fCattle2, 2490);
    }
    {
        const auto& sds = eCattle3SpriteData30;
        SpriteLoader loader(fTileH, "cattle3", sds,
                             &ePoseidonImpsOffset, fRenderer);

        sLoadCattle(loader, fCattle3, 2690);
    }
}

void CharacterTextures::loadBull() {
    if(fBullLoaded) return;
    fBullLoaded = true;
    const auto& sds = eBullSpriteData30;
    SpriteLoader loader(fTileH, "bull", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(2890, 2890, 2986, fBull.fWalk);
    loader.loadSkipFlipped(2890, 2986, 3082, fBull.fStand);

    for(int i = 3082; i < 3090; i++) {
        loader.load(2890, i, fBull.fDie);
    }
    loader.loadSkipFlipped(2890, 3090, 3442, fBull.fAttack);
}

void CharacterTextures::loadButcher() {
    if(fButcherLoaded) return;
    fButcherLoaded = true;
    const auto& sds = eButcherSpriteData30;
    SpriteLoader loader(fTileH, "butcher", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(2186, 2186, 2282, fButcher.fWalk);

    for(int i = 2282; i < 2290; i++) {
        loader.load(2186, i, fButcher.fDie);
    }
}

void CharacterTextures::loadScholar() {
    if(fScholarLoaded) return;
    fScholarLoaded = true;
    const auto& sds = eScholarSpriteData30;
    SpriteLoader loader(fTileH, "scholar", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(2082, 2082, 2178, fScholar.fWalk);

    for(int i = 2178; i < 2186; i++) {
        loader.load(2082, i, fScholar.fDie);
    }
}

void CharacterTextures::loadAstronomer() {
    if(fAstronomerLoaded) return;
    fAstronomerLoaded = true;
    const auto& sds = eAstronomerSpriteData30;
    SpriteLoader loader(fTileH, "astronomer", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(79, 79, 175, fAstronomer.fWalk);

    for(int i = 175; i < 183; i++) {
        loader.load(79, i, fAstronomer.fDie);
    }
}

void CharacterTextures::loadInventor() {
    if(fInventorLoaded) return;
    fInventorLoaded = true;
    const auto& sds = eInventorSpriteData30;
    SpriteLoader loader(fTileH, "inventor", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(3442, 3442, 3538, fInventor.fWalk);

    for(int i = 3538; i < 3546; i++) {
        loader.load(3442, i, fInventor.fDie);
    }
}

void CharacterTextures::loadCurator() {
    if(fCuratorLoaded) return;
    fCuratorLoaded = true;
    const auto& sds = eCuratorSpriteData30;
    SpriteLoader loader(fTileH, "curator", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(79, 79, 175, fCurator.fWalk);

    for(int i = 175; i < 183; i++) {
        loader.load(79, i, fCurator.fDie);
    }
}

void CharacterTextures::loadChariotPoseidon() {
    if(fChariotPoseidonLoaded) return;
    fChariotPoseidonLoaded = true;
    const auto& sds = eChariotPoseidonSpriteData30;
    SpriteLoader loader(fTileH, "chariotPoseidon", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(3910, 3910, 4006, fChariotPoseidon.fWalk);
    loader.loadSkipFlipped(3910, 4078, 4174, fChariotPoseidon.fFight);
    loader.loadSkipFlipped(3910, 4006, 4078, fChariotPoseidon.fDie);
}

void CharacterTextures::loadArcherPoseidon() {
    if(fArcherPoseidonLoaded) return;
    fArcherPoseidonLoaded = true;
    const auto& sds = eArcherPoseidonSpriteData30;
    SpriteLoader loader(fTileH, "archerPoseidon", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(4174, 4174, 4270, fArcherPoseidon.fWalk);
    loader.loadSkipFlipped(4174, 4278, 4398, fArcherPoseidon.fFight);

    for(int i = 4270; i < 4278; i++) {
        loader.load(4174, i, fArcherPoseidon.fDie);
    }
}

void CharacterTextures::loadHoplitePoseidon() {
    if(fHoplitePoseidonLoaded) return;
    fHoplitePoseidonLoaded = true;
    const auto& sds = eHoplitePoseidonSpriteData30;
    SpriteLoader loader(fTileH, "hoplitePoseidon", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(3742, 3742, 3838, fHoplitePoseidon.fWalk);
    loader.loadSkipFlipped(3742, 3846, 3910, fHoplitePoseidon.fFight);

    for(int i = 3838; i < 3846; i++) {
        loader.load(3742, i, fHoplitePoseidon.fDie);
    }
}

void CharacterTextures::loadRacingHorses() {
    if(fRacingHorsesLoaded) return;
    fRacingHorsesLoaded = true;

    {
        const auto& sds = eHippodromeHorse1SpriteData30;
        SpriteLoader loader(fTileH, "hippodromeHorse1", sds,
                             &ePoseidonImpsOffset, fRenderer);

        loader.loadHorseSkipFlipped(5558, 5558, 5766, fRacingHorse1.fRace);
        loader.loadSkipFlipped(5558, 5766, 5870, fRacingHorse1.fStand);

        for(int i = 5870; i < 5879; i++) {
            loader.load(5558, i, fRacingHorse1.fDie);
        }
    }
    {
        const auto& sds = eHippodromeHorse2SpriteData30;
        SpriteLoader loader(fTileH, "hippodromeHorse2", sds,
                             &ePoseidonImpsOffset, fRenderer);

        loader.loadHorseSkipFlipped(5879, 5879, 6087, fRacingHorse2.fRace);
        loader.loadSkipFlipped(5879, 6087, 6247, fRacingHorse2.fStand);

        for(int i = 6247; i < 6256; i++) {
            loader.load(5879, i, fRacingHorse2.fDie);
        }
    }
    {
        const auto& sds = eHippodromeHorse3SpriteData30;
        SpriteLoader loader(fTileH, "hippodromeHorse3", sds,
                             &ePoseidonImpsOffset, fRenderer);

        loader.loadHorseSkipFlipped(6256, 6256, 6464, fRacingHorse3.fRace);
        loader.loadSkipFlipped(6256, 6464, 6568, fRacingHorse3.fStand);

        for(int i = 6568; i < 6577; i++) {
            loader.load(6256, i, fRacingHorse3.fDie);
        }
    }
    {
        const auto& sds = eHippodromeHorse4SpriteData30;
        SpriteLoader loader(fTileH, "hippodromeHorse4", sds,
                             &ePoseidonImpsOffset, fRenderer);

        loader.loadHorseSkipFlipped(6577, 6577, 6785, fRacingHorse4.fRace);
        loader.loadSkipFlipped(6577, 6785, 6945, fRacingHorse4.fStand);

        for(int i = 6945; i < 6954; i++) {
            loader.load(6577, i, fRacingHorse4.fDie);
        }
    }
}
