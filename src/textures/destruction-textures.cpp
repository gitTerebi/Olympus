#include "destruction-textures.h"

#include "offsets/SprAmbient.h"
#include "offsets/destruction.h"
#include "offsets/PoseidonImps.h"

#include "spriteData/fire130.h"

#include "spriteData/plague30.h"

#include "spriteData/bless30.h"

#include "spriteData/cursed30.h"

#include "spriteData/blessed30.h"

#include "spriteData/curse30.h"

#include "spriteData/godOrangeMissile30.h"

#include "spriteData/godBlueArrow30.h"

#include "spriteData/godOrangeArrow30.h"

#include "spriteData/monsterMissile30.h"

#include "spriteData/godBlueMissile30.h"

#include "spriteData/godRedMissile30.h"

#include "spriteData/godGreenMissile30.h"

#include "spriteData/godPinkMissile30.h"

#include "spriteData/godPurpleMissile30.h"

#include "spriteData/rock30.h"

#include "spriteData/blackSpear30.h"

#include "spriteData/spear30.h"

#include "spriteData/blackArrow30.h"

#include "spriteData/arrow30.h"

#include "spriteData/orichalcMissile30.h"

#include "spriteData/wave30.h"

#include "spriteData/lava30.h"

#include "spriteData/dust30.h"

#include "textures/sprite-loader.h"

DestructionTextures::DestructionTextures(const int tileW, const int tileH,
                                           SDL_Renderer* const renderer) :
    fTileW(tileW), fTileH(tileH),
    fRenderer(renderer),

    fBlessed(renderer),
    fCursed(renderer),

    fRock(renderer),
    fBlackSpear(renderer),
    fSpear(renderer),
    fBlackArrow(renderer),
    fArrow(renderer),

    fPlague(renderer),

    fWave(renderer) {}

void DestructionTextures::loadAll() {
    loadFire();

    loadBlessed();
    loadCursed();

    loadRock();
    loadBlackSpear();
    loadSpear();
    loadBlackArrow();
    loadArrow();

    loadBless();
    loadCurse();

    loadPlague();

    loadGodOrangeMissile();
    loadGodBlueArrow();
    loadGodOrangeArrow();
    loadMonsterMissile();
    loadGodBlueMissile();
    loadGodRedMissile();
    loadGodGreenMissile();
    loadGodPinkMissile();
    loadGodPurpleMissile();
    loadOrichalcMissile();
}

void DestructionTextures::loadFire() {
    if(fFireLoaded) return;
    fFireLoaded = true;

    const auto& sds = eFire1SpriteData30;
    SpriteLoader loader(fTileH, "fire1", sds,
                         nullptr/*&eDestructionOffset*/, fRenderer);

    for(int i = 0; i < 3; i++) {
        fFire.emplace_back(fRenderer);
    }

    for(int i = 37; i < 63; i++) {
        loader.load(37, i, fFire[0]);
    }
    for(int i = 63; i < 89; i++) {
        loader.load(37, i, fFire[1]);
    }
//    for(int i = 89; i < 115; i++) {
//        eTextureLoadingHelpers::loadTex(pathBase, i, fFire[2]);
//    }
    for(int i = 115; i < 133; i++) {
        loader.load(37, i, fFire[2]);
    }
//    for(int i = 133; i < 157; i++) {
//        eTextureLoadingHelpers::loadTex(pathBase, i, fFire[4]);
//    }
//    for(int i = 1007; i < 1033; i++) {
//        eTextureLoadingHelpers::loadTex(pathBase, i, fFire[4]);
//    }

//    for(int i = 0; i < 2; i++) {
//        fBigFire.emplace_back(fRenderer);
//    }

//    for(int i = 1033; i < 1059; i++) {
//        eTextureLoadingHelpers::loadTex(pathBase, i, fBigFire[0]);
//    }
//    for(int i = 1059; i < 1085; i++) {
//        eTextureLoadingHelpers::loadTex(pathBase, i, fBigFire[1]);
//    }
}

void DestructionTextures::loadBlessed() {
    if(fBlessedLoaded) return;
    fBlessedLoaded = true;

    const auto& sds = eBlessedSpriteData30;
    SpriteLoader loader(fTileH, "blessed", sds,
                         &eDestructionOffset, fRenderer);
    for(int i = 716; i < 740; i++) {
        loader.load(716, i, fBlessed);
    }
}

void DestructionTextures::loadCursed() {
    if(fCursedLoaded) return;
    fCursedLoaded = true;

    const auto& sds = eCursedSpriteData30;
    SpriteLoader loader(fTileH, "cursed", sds,
                         &eDestructionOffset, fRenderer);

    for(int i = 692; i < 716; i++) {
        loader.load(692, i, fCursed);
    }
}

void DestructionTextures::loadRock() {
    if(fRockLoaded) return;
    fRockLoaded = true;

    const auto& sds = eRockSpriteData30;
    SpriteLoader loader(fTileH, "rock", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 1490; i < 1492; i++) {
        loader.load(1490, i, fRock);
    }
}

void DestructionTextures::loadBlackSpear() {
    if(fBlackSpearLoaded) return;
    fBlackSpearLoaded = true;

    const auto& sds = eBlackSpearSpriteData30;
    SpriteLoader loader(fTileH, "blackSpear", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 2213; i < 2245; i++) {
        loader.load(2213, i, fBlackSpear);
    }
}

void DestructionTextures::loadSpear() {
    if(fSpearLoaded) return;
    fSpearLoaded = true;

    const auto& sds = eSpearSpriteData30;
    SpriteLoader loader(fTileH, "spear", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 2245; i < 2277; i++) {
        loader.load(2245, i, fSpear);
    }
}

void DestructionTextures::loadBlackArrow() {
    if(fBlackArrowLoaded) return;
    fBlackArrowLoaded = true;

    const auto& sds = eBlackArrowSpriteData30;
    SpriteLoader loader(fTileH, "blackArrow", sds,
                         &eSprAmbientOffset, fRenderer);
    loader.loadArrowSkipFlipped(2575, 2575, 2607, fBlackArrow);
}

void DestructionTextures::loadArrow() {
    if(fArrowLoaded) return;
    fArrowLoaded = true;

    const auto& sds = eArrowSpriteData30;
    SpriteLoader loader(fTileH, "arrow", sds,
                         &eSprAmbientOffset, fRenderer);
    loader.loadArrowSkipFlipped(2607, 2607, 2639, fArrow);
}

void DestructionTextures::loadBless() {
    if(fBlessLoaded) return;
    fBlessLoaded = true;

    const auto& sds = eBlessSpriteData30;
    SpriteLoader loader(fTileH, "bless", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(492, 492, 692, fBless);
}

void DestructionTextures::loadCurse() {
    if(fCurseLoaded) return;
    fCurseLoaded = true;

    const auto& sds = eCurseSpriteData30;
    SpriteLoader loader(fTileH, "curse", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(807, 807, 1007, fCurse);
}

void DestructionTextures::loadPlague() {
    if(fPlagueLoaded) return;
    fPlagueLoaded = true;

    const auto& sds = ePlagueSpriteData30;
    SpriteLoader loader(fTileH, "plague", sds,
                         &eDestructionOffset, fRenderer);
    for(int i = 1085; i < 1103; i++) {
        loader.load(1085, i, fPlague);
    }
}

void DestructionTextures::loadGodOrangeMissile() {
    if(fGodOrangeMissileLoaded) return;
    fGodOrangeMissileLoaded = true;

    const auto& sds = eGodOrangeMissileSpriteData30;
    SpriteLoader loader(fTileH, "godOrangeMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1103, 1103, 1167, fGodOrangeMissile);
}

void DestructionTextures::loadGodBlueArrow() {
    if(fGodBlueArrowLoaded) return;
    fGodBlueArrowLoaded = true;

    const auto& sds = eGodBlueArrowSpriteData30;
    SpriteLoader loader(fTileH, "godBlueArrow", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1212, 1212, 1276, fGodBlueArrow);
}

void DestructionTextures::loadGodOrangeArrow() {
    if(fGodOrangeArrowLoaded) return;
    fGodOrangeArrowLoaded = true;

    const auto& sds = eGodOrangeArrowSpriteData30;
    SpriteLoader loader(fTileH, "godOrangeArrow", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1276, 1276, 1340, fGodOrangeArrow);
}

void DestructionTextures::loadMonsterMissile() {
    if(fMonsterMissileLoaded) return;
    fMonsterMissileLoaded = true;

    const auto& sds = eMonsterMissileSpriteData30;
    SpriteLoader loader(fTileH, "monsterMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1340, 1340, 1404, fMonsterMissile);
}

void DestructionTextures::loadGodBlueMissile() {
    if(fGodBlueMissileLoaded) return;
    fGodBlueMissileLoaded = true;

    const auto& sds = eGodBlueMissileSpriteData30;
    SpriteLoader loader(fTileH, "godBlueMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1404, 1404, 1468, fGodBlueMissile);
}

void DestructionTextures::loadGodRedMissile() {
    if(fGodRedMissileLoaded) return;
    fGodRedMissileLoaded = true;

    const auto& sds = eGodRedMissileSpriteData30;
    SpriteLoader loader(fTileH, "godRedMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1468, 1468, 1532, fGodRedMissile);
}

void DestructionTextures::loadGodGreenMissile() {
    if(fGodGreenMissileLoaded) return;
    fGodGreenMissileLoaded = true;

    const auto& sds = eGodGreenMissileSpriteData30;
    SpriteLoader loader(fTileH, "godGreenMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1532, 1532, 1596, fGodGreenMissile);
}

void DestructionTextures::loadGodPinkMissile() {
    if(fGodPinkMissileLoaded) return;
    fGodPinkMissileLoaded = true;

    const auto& sds = eGodPinkMissileSpriteData30;
    SpriteLoader loader(fTileH, "godPinkMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1596, 1596, 1660, fGodPinkMissile);
}

void DestructionTextures::loadGodPurpleMissile() {
    if(fGodPurpleMissileLoaded) return;
    fGodPurpleMissileLoaded = true;

    const auto& sds = eGodPurpleMissileSpriteData30;
    SpriteLoader loader(fTileH, "godPurpleMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1750, 1750, 1814, fGodPurpleMissile);
}

void DestructionTextures::loadOrichalcMissile() {
    if(fOrichalcMissileLoaded) return;
    fOrichalcMissileLoaded = true;

    const auto& sds = eOrichalcMissileSpriteData30;
    SpriteLoader loader(fTileH, "orichalcMissile", sds,
                         &ePoseidonImpsOffset, fRenderer);
    loader.loadSkipFlipped(4718, 4718, 4758, fOrichalcMissile);
}

void DestructionTextures::loadWave() {
    if(fWaveLoaded) return;
    fWaveLoaded = true;

    const auto& sds = eWaveSpriteData30;
    SpriteLoader loader(fTileH, "wave", sds,
                         &eDestructionOffset, fRenderer);

    for(int i = 1660; i < 1686; i++) {
        loader.load(1660, i, fWave);
    }
}

void DestructionTextures::loadLava() {
    if(fLavaLoaded) return;
    fLavaLoaded = true;

    const auto& sds = eLavaSpriteData30;
    SpriteLoader loader(fTileH, "lava", sds,
                         &eDestructionOffset, fRenderer);

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 165; i < 184; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 184; i < 202; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 202; i < 219; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 219; i < 237; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 237; i < 255; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 255; i < 274; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 274; i < 286; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 286; i < 299; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 299; i < 314; i++) {
            loader.load(165, i, coll);
        }
    }

    {
        auto& coll = fLava.emplace_back(fRenderer);
        for(int i = 314; i < 329; i++) {
            loader.load(165, i, coll);
        }
    }
}

void DestructionTextures::loadDust() {
    if(fDustLoaded) return;
    fDustLoaded = true;

    const auto& sds = eDustSpriteData30;
    SpriteLoader loader(fTileH, "dust", sds,
                         &eDestructionOffset, fRenderer);

    {
        auto& coll = fDust.emplace_back(fRenderer);
        for(int i = 1; i < 10; i++) {
            loader.load(1, i, coll);
        }
    }
    {
        auto& coll = fDust.emplace_back(fRenderer);
        for(int i = 10; i < 19; i++) {
            loader.load(1, i, coll);
        }
    }
    {
        auto& coll = fDust.emplace_back(fRenderer);
        for(int i = 19; i < 28; i++) {
            loader.load(1, i, coll);
        }
    }
    {
        auto& coll = fDust.emplace_back(fRenderer);
        for(int i = 28; i < 37; i++) {
            loader.load(1, i, coll);
        }
    }
}
