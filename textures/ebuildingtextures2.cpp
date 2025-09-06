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
