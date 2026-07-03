#include "building-textures.h"

#include "spriteData/bibliotheke30.h"

#include "spriteData/observatory30.h"

#include "spriteData/university30.h"

#include "spriteData/laboratory30.h"

#include "spriteData/inventorsWorkshop30.h"

#include "spriteData/museum30.h"

#include "spriteData/corral30.h"

#include "spriteData/corralCattleOverlay30.h"

#include "spriteData/corralProcessingOverlay30.h"

#include "spriteData/chariotFactory30.h"

#include "spriteData/chariotFactoryOverlay130.h"

#include "spriteData/chariotFactoryHorses30.h"

#include "spriteData/poseidonCommonHouse30.h"

#include "spriteData/poseidonEliteHouse30.h"

#include "spriteData/poseidonSanctuary30.h"

#include "spriteData/poseidonHerosHall230.h"

#include "spriteData/altarBullOverlay30.h"

#include "spriteData/blackMarbleWorkshop30.h"

#include "spriteData/refinery30.h"

#include "spriteData/orichalcTowerOverlay30.h"

#include "spriteData/poseidonBridge30.h"

#include "spriteData/poseidonPyramid30.h"

#include "spriteData/poseidonPyramid230.h"

#include "offsets/PoseidonImps.h"

#include "textures/sprite-loader.h"

void BuildingTextures::loadPyramid() {
    if(fPyramidLoaded) return;
    fPyramidLoaded = true;

    const auto& sds = ePoseidonPyramidSpriteData30;
    SpriteLoader loader(fTileH, "poseidonPyramid", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 46; i++) {
        loader.load(1, i, fPyramid);
    }
}

void BuildingTextures::loadPyramid2() {
    if(fPyramid2Loaded) return;
    fPyramid2Loaded = true;

    const auto& sds = ePoseidonPyramid2SpriteData30;
    SpriteLoader loader(fTileH, "poseidonPyramid2", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 39; i++) {
        loader.load(1, i, fPyramid2);
    }
}

void BuildingTextures::loadPoseidonBridge() {
    if(fPoseidonBridgeLoaded) return;
    fPoseidonBridgeLoaded = true;

    const auto& sds = ePoseidonBridgeSpriteData30;
    SpriteLoader loader(fTileH, "poseidonBridge", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 17; i++) {
        loader.load(1, i, fPoseidonBridge);
    }
}

void BuildingTextures::loadOrichalcTowerOverlay() {
    if(fOrichalcTowerOverlayLoaded) return;
    fOrichalcTowerOverlayLoaded = true;

    const auto& sds = eOrichalcTowerOverlaySpriteData30;
    SpriteLoader loader(fTileH, "orichalcTowerOverlay", sds,
                         nullptr, fRenderer);

    fOrichalcTowerOverlay = loader.load(45, 45);
}

void BuildingTextures::loadRefinery() {
    if(fRefineryLoaded) return;
    fRefineryLoaded = true;

    const auto& sds = eRefinerySpriteData30;
    SpriteLoader loader(fTileH, "refinery", sds,
                         nullptr, fRenderer);

    fRefinery = loader.load(1, 1);
    for(int i = 2; i < 15; i++) {
        loader.load(1, i, fRefineryOverlay);
    }
}

void BuildingTextures::loadBlackMarbleWorkshop() {
    if(fBlackMarbleWorkshopLoaded) return;
    fBlackMarbleWorkshopLoaded = true;

    const auto& sds = eBlackMarbleWorkshopSpriteData30;
    SpriteLoader loader(fTileH, "blackMarbleWorkshop", sds,
                         nullptr, fRenderer);

    fBlackMarbleWorkshop = loader.load(36, 36);
    for(int i = 37; i < 45; i++) {
        loader.load(36, i, fBlackMarbleWorkshopStones);
    }

    loadMasonryShopOverlays();
}

void BuildingTextures::loadAltarBullOverlay() {
    if(fAltarBullOverlayLoaded) return;
    fAltarBullOverlayLoaded = true;

    const auto& sds = eAltarBullOverlaySpriteData30;
    SpriteLoader loader(fTileH, "altarBullOverlay", sds,
                         nullptr, fRenderer);

    for(int i = 3650; i < 3674; i++) {
        loader.load(3650, i, fAltarBullOverlay);
    }
    generateFlipped(fAltarBullOverlay, fAltarBullOverlayFlipped);
}

void BuildingTextures::loadPoseidonHerosHall() {
    if(fPoseidonHerosHallLoaded) return;
    fPoseidonHerosHallLoaded = true;

    const auto& sds = ePoseidonHerosHall2SpriteData30;
    SpriteLoader loader(fTileH, "poseidonHerosHall2", sds,
                         nullptr, fRenderer);

    fPoseidonHeroHall = loader.load(1, 1);
}

void BuildingTextures::loadPoseidonSanctuary() {
    if(fPoseidonSanctuaryLoaded) return;
    fPoseidonSanctuaryLoaded = true;

    const auto& sds = ePoseidonSanctuarySpriteData30;
    SpriteLoader loader(fTileH, "poseidonSanctuary", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 5; i++) {
        loader.load(1, i, fPoseidonSanctuary);
    }
}

void BuildingTextures::loadPoseidonCommonHouse() {
    if(fPoseidonCommonHouseLoaded) return;
    fPoseidonCommonHouseLoaded = true;

    const auto& sds = ePoseidonCommonHouseSpriteData30;
    SpriteLoader loader(fTileH, "poseidonCommonHouse", sds,
                         nullptr, fRenderer);

    for(int i = 1; i < 11;) {
        auto& coll = fPoseidonCommonHouse.emplace_back(fRenderer);
        for(int j = 0; j < 2; j++, i++) {
            loader.load(1, i, coll);
        }
    }
}

void BuildingTextures::loadPoseidonEliteHouse() {
    if(fPoseidonEliteHouseLoaded) return;
    fPoseidonEliteHouseLoaded = true;
    const auto& sds = ePoseidonEliteHouseSpriteData30;
    SpriteLoader loader(fTileH, "poseidonEliteHouse", sds,
                         nullptr, fRenderer);

    for(int i = 11; i < 41;) {
        auto& coll = fPoseidonEliteHouse.emplace_back(fRenderer);
        for(int j = 0; j < 5; j++, i++) {
            loader.load(11, i, coll);
        }
    }
}

void BuildingTextures::loadChariotFactory() {
    if(fChariotFactoryLoaded) return;
    fChariotFactoryLoaded = true;

    {
        const auto& sds = eChariotFactorySpriteData30;
        SpriteLoader loader(fTileH, "chariotFactory", sds,
                             nullptr, fRenderer);
        fChariotFactory = loader.load(15, 15);

        for(int i = 16; i < 36; i++) {
            loader.load(15, i, fChariotFactoryOverlay);
        }
    }
    {
        const auto& sds = eChariotFactoryOverlay1SpriteData30;
        SpriteLoader loader(fTileH, "chariotFactoryOverlay1", sds,
                             &ePoseidonImpsOffset, fRenderer);

        for(int i = 3718; i < 3742; i++) {
            loader.load(3718, i, fChariotFactoryOverlay1);
        }
    }
    {
        const auto& sds = eChariotFactoryHorsesSpriteData30;
        SpriteLoader loader(fTileH, "chariotFactoryHorses", sds,
                             &ePoseidonImpsOffset, fRenderer);

        for(int i = 5374; i < 5558;) {
            auto& coll = fChariotFactoryHorses.emplace_back(fRenderer);
            for(int j = 0; j < 46; j++, i++) {
                loader.load(5374, i, coll);
            }
        }
    }
}

void BuildingTextures::loadBibliotheke() {
    if(fBibliothekeLoaded) return;
    fBibliothekeLoaded = true;

    const auto& sds = eBibliothekeSpriteData30;
    SpriteLoader loader(fTileH, "bibliotheke", sds,
                         nullptr, fRenderer);
    fBibliotheke = loader.load(105, 105);

    for(int i = 106; i < 141; i++) {
        loader.load(105, i, fBibliothekeOverlay);
    }
}

void BuildingTextures::loadObservatory() {
    if(fObservatoryLoaded) return;
    fObservatoryLoaded = true;

    const auto& sds = eObservatorySpriteData30;
    SpriteLoader loader(fTileH, "observatory", sds,
                         nullptr, fRenderer);
    fObservatory = loader.load(141, 141);

    for(int i = 142; i < 162; i++) {
        loader.load(141, i, fObservatoryOverlay);
    }
}

void BuildingTextures::loadUniversity() {
    if(fUniversityLoaded) return;
    fUniversityLoaded = true;

    const auto& sds = eUniversitySpriteData30;
    SpriteLoader loader(fTileH, "university", sds,
                         nullptr, fRenderer);
    fUniversity = loader.load(1, 1);

    for(int i = 2; i < 26; i++) {
        loader.load(1, i, fUniversityOverlay);
    }
}

void BuildingTextures::loadLaboratory() {
    if(fLaboratoryLoaded) return;
    fLaboratoryLoaded = true;

    const auto& sds = eLaboratorySpriteData30;
    SpriteLoader loader(fTileH, "laboratory", sds,
                         nullptr, fRenderer);
    fLaboratory = loader.load(26, 26);

    for(int i = 27; i < 67; i++) {
        loader.load(26, i, fLaboratoryOverlay);
    }
}

void BuildingTextures::loadInventorsWorkshop() {
    if(fInventorsWorkshopLoaded) return;
    fInventorsWorkshopLoaded = true;

    const auto& sds = eInventorsWorkshopSpriteData30;
    SpriteLoader loader(fTileH, "inventorsWorkshop", sds,
                         nullptr, fRenderer);
    fInventorsWorkshop = loader.load(67, 67);

    for(int i = 68; i < 105; i++) {
        loader.load(67, i, fInventorsWorkshopOverlay);
    }
}

void BuildingTextures::loadMuseum() {
    if(fMuseumLoaded) return;
    fMuseumLoaded = true;

    const auto& sds = eMuseumSpriteData30;
    SpriteLoader loader(fTileH, "museum", sds,
                         nullptr, fRenderer);
    fMuseum = loader.load(162, 162);

    for(int i = 163; i < 185; i++) {
        loader.load(162, i, fMuseumOverlay);
    }
}

void BuildingTextures::loadCorral() {
    if(fCorralLoaded) return;
    fCorralLoaded = true;

    {
        const auto& sds = eCorralSpriteData30;
        SpriteLoader loader(fTileH, "corral", sds,
                             nullptr, fRenderer);
        fCorral = loader.load(1, 1);

        for(int i = 2; i < 49; i++) {
            loader.load(1, i, fCorralOverlay);
        }
    }
    {
        const auto& sds = eCorralCattleOverlaySpriteData30;
        SpriteLoader loader(fTileH, "corralCattleOverlay", sds,
                             nullptr, fRenderer);

        for(int i = 0, j = 0; i < 3; i++) {
            auto& o = fCorralCattleOverlay.emplace_back(fRenderer);
            for(int k = 0; k < 21; k++, j++) {
                loader.load(1815, 1815 + j, o);
            }
        }
    }
    {
        const auto& sds = eCorralProcessingOverlaySpriteData30;
        SpriteLoader loader(fTileH, "corralProcessingOverlay", sds,
                             nullptr, fRenderer);

        for(int i = 1878; i < 1938; i++) {
            loader.load(1878, i, fCorralProcessingOverlay);
        }
    }
}
