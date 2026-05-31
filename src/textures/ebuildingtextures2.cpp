#include "ebuildingtextures.h"

#include "spriteData/hippodromeFinish15.h"
#include "spriteData/hippodromeFinish30.h"
#include "spriteData/hippodromeFinish45.h"
#include "spriteData/hippodromeFinish60.h"

#include "spriteData/hippodromeSpectators15.h"
#include "spriteData/hippodromeSpectators30.h"
#include "spriteData/hippodromeSpectators45.h"
#include "spriteData/hippodromeSpectators60.h"

#include "spriteData/hippodromeFeces15.h"
#include "spriteData/hippodromeFeces30.h"
#include "spriteData/hippodromeFeces45.h"
#include "spriteData/hippodromeFeces60.h"

#include "offsets/PoseidonImps2.h"

#include "textures/espriteloader.h"

void eBuildingTextures::loadHippodromeSpectators() {
    if(fHippodromeSpectatorsLoaded) return;
    fHippodromeSpectatorsLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHippodromeSpectatorsSpriteData15,
                                 eHippodromeSpectatorsSpriteData30,
                                 eHippodromeSpectatorsSpriteData45,
                                 eHippodromeSpectatorsSpriteData60);
    eSpriteLoader loader(fTileH, "hippodromeSpectators", sds,
                         &ePoseidonImps2Offset, fRenderer);

    for(int j = 993; j < 1121;) {
        auto& coll = fHippodromeSpectators.emplace_back(fRenderer);
        for(int k = 0; k < 32; k++, j++) {
            loader.load(993, j, coll);
        }
    }
}

void eBuildingTextures::loadHippodromeFeces() {
    if(fHippodromeFecesLoaded) return;
    fHippodromeFecesLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHippodromeFecesSpriteData15,
                                 eHippodromeFecesSpriteData30,
                                 eHippodromeFecesSpriteData45,
                                 eHippodromeFecesSpriteData60);
    eSpriteLoader loader(fTileH, "hippodromeFeces", sds,
                         &ePoseidonImps2Offset, fRenderer);

    fHippodromeFeces.emplace_back(fRenderer);
    fHippodromeFeces.emplace_back(fRenderer);

    {
        auto& coll1 = fHippodromeFeces[0];
        auto& coll2 = fHippodromeFeces[1];
        for(int j = 327; j < 625;) {
            for(int k = 0; k < 2; k++, j++) {
                loader.load(327, j, k == 0 ? coll1 : coll2);
            }
        }
    }

    fHippodromeFecesStanding.emplace_back(fRenderer);
    fHippodromeFecesStanding.emplace_back(fRenderer);

    {
        auto& coll1 = fHippodromeFecesStanding[0];
        auto& coll2 = fHippodromeFecesStanding[1];
        for(int j = 625; j < 671;) {
            for(int k = 0; k < 2; k++, j++) {
                loader.load(327, j, k == 0 ? coll1 : coll2);
            }
        }
    }
}

void eBuildingTextures::loadHippodromeFinish() {
    if(fHippodromeFinishLoaded) return;
    fHippodromeFinishLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHippodromeFinishSpriteData15,
                                 eHippodromeFinishSpriteData30,
                                 eHippodromeFinishSpriteData45,
                                 eHippodromeFinishSpriteData60);
    eSpriteLoader loader(fTileH, "hippodromeFinish", sds,
                         &ePoseidonImps2Offset, fRenderer);

    {
        fHippodromeFinishRacing.emplace_back(fRenderer);
        fHippodromeFinishRacing.emplace_back(fRenderer);
        fHippodromeFinishRacing.emplace_back(fRenderer);
        fHippodromeFinishRacing.emplace_back(fRenderer);

        auto& coll1 = fHippodromeFinishRacing[0];
        auto& coll2 = fHippodromeFinishRacing[1];
        auto& coll3 = fHippodromeFinishRacing[2];
        auto& coll4 = fHippodromeFinishRacing[3];
        for(int j = 1121; j < 1213;) {
            for(int k = 0; k < 4; k++, j++) {
                if(k == 2) continue;
                loader.load(1121, j, k == 0 ? coll1 : (k == 1 ? coll2 : coll4));
            }
        }

        generateFlipped(coll4, coll3);
    }
    {
        fHippodromeFinishNotRacing.emplace_back(fRenderer);
        fHippodromeFinishNotRacing.emplace_back(fRenderer);
        fHippodromeFinishNotRacing.emplace_back(fRenderer);
        fHippodromeFinishNotRacing.emplace_back(fRenderer);

        auto& coll1 = fHippodromeFinishNotRacing[0];
        auto& coll2 = fHippodromeFinishNotRacing[1];
        auto& coll3 = fHippodromeFinishNotRacing[2];
        auto& coll4 = fHippodromeFinishNotRacing[3];
        for(int j = 1213; j < 1413;) {
            for(int k = 0; k < 4; k++, j++) {
                if(k == 2) continue;
                loader.load(1121, j, k == 0 ? coll1 : (k == 1 ? coll2 : coll4));
            }
        }

        generateFlipped(coll4, coll3);
    }
}
