#include "echaractertextures.h"

#include "espriteloader.h"

#include "spriteData/trireme15.h"
#include "spriteData/trireme30.h"
#include "spriteData/trireme45.h"
#include "spriteData/trireme60.h"

#include "spriteData/enemyBoat15.h"
#include "spriteData/enemyBoat30.h"
#include "spriteData/enemyBoat45.h"
#include "spriteData/enemyBoat60.h"

#include "spriteData/eliteCitizen15.h"
#include "spriteData/eliteCitizen30.h"
#include "spriteData/eliteCitizen45.h"
#include "spriteData/eliteCitizen60.h"

#include "offsets/SprMain.h"

void eCharacterTextures::loadTrireme() {
    if(fTriremeLoaded) return;
    fTriremeLoaded = true;
    const auto& sds = spriteData(fTileH,
                                 eTriremeSpriteData15,
                                 eTriremeSpriteData30,
                                 eTriremeSpriteData45,
                                 eTriremeSpriteData60);
    eSpriteLoader loader(fTileH, "trireme", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadBoatSkipFlipped(6569, 6570, 6633, fTrireme.fSwim);
    loader.loadSkipFlipped(6569, 6633, 6729, fTrireme.fDie);
    loader.loadSkipFlipped(6569, 6729, 6737, fTrireme.fStand);
}

void eCharacterTextures::loadEnemyBoat() {
    if(fEnemyBoatLoaded) return;
    fEnemyBoatLoaded = true;
    const auto& sds = spriteData(fTileH,
                                 eEnemyBoatSpriteData15,
                                 eEnemyBoatSpriteData30,
                                 eEnemyBoatSpriteData45,
                                 eEnemyBoatSpriteData60);
    eSpriteLoader loader(fTileH, "enemyBoat", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadBoatSkipFlipped(4833, 4834, 4897, fEnemyBoat.fSwim);
    loader.loadSkipFlipped(4833, 4897, 4993, fEnemyBoat.fDie);
    loader.loadSkipFlipped(4833, 4993, 5001, fEnemyBoat.fStand);
}

void eCharacterTextures::loadEliteCitizen() {
    if(fEliteCitizenLoaded) return;
    fEliteCitizenLoaded = true;
    const auto& sds = spriteData(fTileH,
                                 eEliteCitizenSpriteData15,
                                 eEliteCitizenSpriteData30,
                                 eEliteCitizenSpriteData45,
                                 eEliteCitizenSpriteData60);
    eSpriteLoader loader(fTileH, "eliteCitizen", sds,
                         &eSprMainOffset, fRenderer);

    loader.loadSkipFlipped(6201, 6201, 6297, fEliteCitizen.fWalk);

    for(int i = 6297; i < 6305; i++) {
        loader.load(6201, i, fEliteCitizen.fDie);
    }

    loader.loadSkipFlipped(6201, 6305, 6401, fEliteCitizen.fFight);
}
