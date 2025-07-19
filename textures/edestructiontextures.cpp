#include "edestructiontextures.h"

#include "offsets/SprAmbient.h"
#include "offsets/destruction.h"
#include "offsets/PoseidonImps.h"

#include "spriteData/fire115.h"
#include "spriteData/fire130.h"
#include "spriteData/fire145.h"
#include "spriteData/fire160.h"

#include "spriteData/plague15.h"
#include "spriteData/plague30.h"
#include "spriteData/plague45.h"
#include "spriteData/plague60.h"

#include "spriteData/bless15.h"
#include "spriteData/bless30.h"
#include "spriteData/bless45.h"
#include "spriteData/bless60.h"

#include "spriteData/cursed15.h"
#include "spriteData/cursed30.h"
#include "spriteData/cursed45.h"
#include "spriteData/cursed60.h"

#include "spriteData/blessed15.h"
#include "spriteData/blessed30.h"
#include "spriteData/blessed45.h"
#include "spriteData/blessed60.h"

#include "spriteData/curse15.h"
#include "spriteData/curse30.h"
#include "spriteData/curse45.h"
#include "spriteData/curse60.h"

#include "spriteData/godOrangeMissile15.h"
#include "spriteData/godOrangeMissile30.h"
#include "spriteData/godOrangeMissile45.h"
#include "spriteData/godOrangeMissile60.h"

#include "spriteData/godBlueArrow15.h"
#include "spriteData/godBlueArrow30.h"
#include "spriteData/godBlueArrow45.h"
#include "spriteData/godBlueArrow60.h"

#include "spriteData/godOrangeArrow15.h"
#include "spriteData/godOrangeArrow30.h"
#include "spriteData/godOrangeArrow45.h"
#include "spriteData/godOrangeArrow60.h"

#include "spriteData/monsterMissile15.h"
#include "spriteData/monsterMissile30.h"
#include "spriteData/monsterMissile45.h"
#include "spriteData/monsterMissile60.h"

#include "spriteData/godBlueMissile15.h"
#include "spriteData/godBlueMissile30.h"
#include "spriteData/godBlueMissile45.h"
#include "spriteData/godBlueMissile60.h"

#include "spriteData/godRedMissile15.h"
#include "spriteData/godRedMissile30.h"
#include "spriteData/godRedMissile45.h"
#include "spriteData/godRedMissile60.h"

#include "spriteData/godGreenMissile15.h"
#include "spriteData/godGreenMissile30.h"
#include "spriteData/godGreenMissile45.h"
#include "spriteData/godGreenMissile60.h"

#include "spriteData/godPinkMissile15.h"
#include "spriteData/godPinkMissile30.h"
#include "spriteData/godPinkMissile45.h"
#include "spriteData/godPinkMissile60.h"

#include "spriteData/godPurpleMissile15.h"
#include "spriteData/godPurpleMissile30.h"
#include "spriteData/godPurpleMissile45.h"
#include "spriteData/godPurpleMissile60.h"

#include "spriteData/rock15.h"
#include "spriteData/rock30.h"
#include "spriteData/rock45.h"
#include "spriteData/rock60.h"

#include "spriteData/blackSpear15.h"
#include "spriteData/blackSpear30.h"
#include "spriteData/blackSpear45.h"
#include "spriteData/blackSpear60.h"

#include "spriteData/spear15.h"
#include "spriteData/spear30.h"
#include "spriteData/spear45.h"
#include "spriteData/spear60.h"

#include "spriteData/blackArrow15.h"
#include "spriteData/blackArrow30.h"
#include "spriteData/blackArrow45.h"
#include "spriteData/blackArrow60.h"

#include "spriteData/arrow15.h"
#include "spriteData/arrow30.h"
#include "spriteData/arrow45.h"
#include "spriteData/arrow60.h"

#include "spriteData/orichalcMissile15.h"
#include "spriteData/orichalcMissile30.h"
#include "spriteData/orichalcMissile45.h"
#include "spriteData/orichalcMissile60.h"

#include "textures/espriteloader.h"

eDestructionTextures::eDestructionTextures(const int tileW, const int tileH,
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

    fPlague(renderer) {

}

void eDestructionTextures::loadAll() {
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

void eDestructionTextures::loadFire() {
    if(fFireLoaded) return;
    fFireLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eFire1SpriteData15,
                                 eFire1SpriteData30,
                                 eFire1SpriteData45,
                                 eFire1SpriteData60);
    eSpriteLoader loader(fTileH, "fire1", sds,
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

void eDestructionTextures::loadBlessed() {
    if(fBlessedLoaded) return;
    fBlessedLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eBlessedSpriteData15,
                                 eBlessedSpriteData30,
                                 eBlessedSpriteData45,
                                 eBlessedSpriteData60);
    eSpriteLoader loader(fTileH, "blessed", sds,
                         &eDestructionOffset, fRenderer);
    for(int i = 716; i < 740; i++) {
        loader.load(716, i, fBlessed);
    }
}

void eDestructionTextures::loadCursed() {
    if(fCursedLoaded) return;
    fCursedLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eCursedSpriteData15,
                                 eCursedSpriteData30,
                                 eCursedSpriteData45,
                                 eCursedSpriteData60);
    eSpriteLoader loader(fTileH, "cursed", sds,
                         &eDestructionOffset, fRenderer);

    for(int i = 692; i < 716; i++) {
        loader.load(692, i, fCursed);
    }
}

void eDestructionTextures::loadRock() {
    if(fRockLoaded) return;
    fRockLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eRockSpriteData15,
                                 eRockSpriteData30,
                                 eRockSpriteData45,
                                 eRockSpriteData60);
    eSpriteLoader loader(fTileH, "rock", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 1490; i < 1492; i++) {
        loader.load(1490, i, fRock);
    }
}

void eDestructionTextures::loadBlackSpear() {
    if(fBlackSpearLoaded) return;
    fBlackSpearLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eBlackSpearSpriteData15,
                                 eBlackSpearSpriteData30,
                                 eBlackSpearSpriteData45,
                                 eBlackSpearSpriteData60);
    eSpriteLoader loader(fTileH, "blackSpear", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 2213; i < 2245; i++) {
        loader.load(2213, i, fBlackSpear);
    }
}

void eDestructionTextures::loadSpear() {
    if(fSpearLoaded) return;
    fSpearLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eSpearSpriteData15,
                                 eSpearSpriteData30,
                                 eSpearSpriteData45,
                                 eSpearSpriteData60);
    eSpriteLoader loader(fTileH, "spear", sds,
                         &eSprAmbientOffset, fRenderer);

    for(int i = 2245; i < 2277; i++) {
        loader.load(2245, i, fSpear);
    }
}

void eDestructionTextures::loadBlackArrow() {
    if(fBlackArrowLoaded) return;
    fBlackArrowLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eBlackArrowSpriteData15,
                                 eBlackArrowSpriteData30,
                                 eBlackArrowSpriteData45,
                                 eBlackArrowSpriteData60);
    eSpriteLoader loader(fTileH, "blackArrow", sds,
                         &eSprAmbientOffset, fRenderer);
    loader.loadArrowSkipFlipped(2575, 2575, 2607, fBlackArrow);
}

void eDestructionTextures::loadArrow() {
    if(fArrowLoaded) return;
    fArrowLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eArrowSpriteData15,
                                 eArrowSpriteData30,
                                 eArrowSpriteData45,
                                 eArrowSpriteData60);
    eSpriteLoader loader(fTileH, "arrow", sds,
                         &eSprAmbientOffset, fRenderer);
    loader.loadArrowSkipFlipped(2607, 2607, 2639, fArrow);
}

void eDestructionTextures::loadBless() {
    if(fBlessLoaded) return;
    fBlessLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eBlessSpriteData15,
                                 eBlessSpriteData30,
                                 eBlessSpriteData45,
                                 eBlessSpriteData60);
    eSpriteLoader loader(fTileH, "bless", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(492, 492, 692, fBless);
}

void eDestructionTextures::loadCurse() {
    if(fCurseLoaded) return;
    fCurseLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eCurseSpriteData15,
                                 eCurseSpriteData30,
                                 eCurseSpriteData45,
                                 eCurseSpriteData60);
    eSpriteLoader loader(fTileH, "curse", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(807, 807, 1007, fCurse);
}

void eDestructionTextures::loadPlague() {
    if(fPlagueLoaded) return;
    fPlagueLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 ePlagueSpriteData15,
                                 ePlagueSpriteData30,
                                 ePlagueSpriteData45,
                                 ePlagueSpriteData60);
    eSpriteLoader loader(fTileH, "plague", sds,
                         &eDestructionOffset, fRenderer);
    for(int i = 1085; i < 1103; i++) {
        loader.load(1085, i, fPlague);
    }
}

void eDestructionTextures::loadGodOrangeMissile() {
    if(fGodOrangeMissileLoaded) return;
    fGodOrangeMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodOrangeMissileSpriteData15,
                                 eGodOrangeMissileSpriteData30,
                                 eGodOrangeMissileSpriteData45,
                                 eGodOrangeMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godOrangeMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1103, 1103, 1167, fGodOrangeMissile);
}

void eDestructionTextures::loadGodBlueArrow() {
    if(fGodBlueArrowLoaded) return;
    fGodBlueArrowLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodBlueArrowSpriteData15,
                                 eGodBlueArrowSpriteData30,
                                 eGodBlueArrowSpriteData45,
                                 eGodBlueArrowSpriteData60);
    eSpriteLoader loader(fTileH, "godBlueArrow", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1212, 1212, 1276, fGodBlueArrow);
}

void eDestructionTextures::loadGodOrangeArrow() {
    if(fGodOrangeArrowLoaded) return;
    fGodOrangeArrowLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodOrangeArrowSpriteData15,
                                 eGodOrangeArrowSpriteData30,
                                 eGodOrangeArrowSpriteData45,
                                 eGodOrangeArrowSpriteData60);
    eSpriteLoader loader(fTileH, "godOrangeArrow", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1276, 1276, 1340, fGodOrangeArrow);
}

void eDestructionTextures::loadMonsterMissile() {
    if(fMonsterMissileLoaded) return;
    fMonsterMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eMonsterMissileSpriteData15,
                                 eMonsterMissileSpriteData30,
                                 eMonsterMissileSpriteData45,
                                 eMonsterMissileSpriteData60);
    eSpriteLoader loader(fTileH, "monsterMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1340, 1340, 1404, fMonsterMissile);
}

void eDestructionTextures::loadGodBlueMissile() {
    if(fGodBlueMissileLoaded) return;
    fGodBlueMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodBlueMissileSpriteData15,
                                 eGodBlueMissileSpriteData30,
                                 eGodBlueMissileSpriteData45,
                                 eGodBlueMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godBlueMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1404, 1404, 1468, fGodBlueMissile);
}

void eDestructionTextures::loadGodRedMissile() {
    if(fGodRedMissileLoaded) return;
    fGodRedMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodRedMissileSpriteData15,
                                 eGodRedMissileSpriteData30,
                                 eGodRedMissileSpriteData45,
                                 eGodRedMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godRedMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1468, 1468, 1532, fGodRedMissile);
}

void eDestructionTextures::loadGodGreenMissile() {
    if(fGodGreenMissileLoaded) return;
    fGodGreenMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodGreenMissileSpriteData15,
                                 eGodGreenMissileSpriteData30,
                                 eGodGreenMissileSpriteData45,
                                 eGodGreenMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godGreenMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1532, 1532, 1596, fGodGreenMissile);
}

void eDestructionTextures::loadGodPinkMissile() {
    if(fGodPinkMissileLoaded) return;
    fGodPinkMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodPinkMissileSpriteData15,
                                 eGodPinkMissileSpriteData30,
                                 eGodPinkMissileSpriteData45,
                                 eGodPinkMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godPinkMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1596, 1596, 1660, fGodPinkMissile);
}

void eDestructionTextures::loadGodPurpleMissile() {
    if(fGodPurpleMissileLoaded) return;
    fGodPurpleMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eGodPurpleMissileSpriteData15,
                                 eGodPurpleMissileSpriteData30,
                                 eGodPurpleMissileSpriteData45,
                                 eGodPurpleMissileSpriteData60);
    eSpriteLoader loader(fTileH, "godPurpleMissile", sds,
                         &eDestructionOffset, fRenderer);
    loader.loadSkipFlipped(1750, 1750, 1814, fGodPurpleMissile);
}

void eDestructionTextures::loadOrichalcMissile() {
    if(fOrichalcMissileLoaded) return;
    fOrichalcMissileLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eOrichalcMissileSpriteData15,
                                 eOrichalcMissileSpriteData30,
                                 eOrichalcMissileSpriteData45,
                                 eOrichalcMissileSpriteData60);
    eSpriteLoader loader(fTileH, "orichalcMissile", sds,
                         &ePoseidonImpsOffset, fRenderer);
    loader.loadSkipFlipped(4718, 4718, 4758, fOrichalcMissile);
}
