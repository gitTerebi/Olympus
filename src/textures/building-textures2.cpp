#include "building-textures.h"

#include "spriteData/hippodromeFinish30.h"

#include "spriteData/hippodromeSpectators30.h"

#include "spriteData/hippodromeFeces30.h"

#include "offsets/PoseidonImps2.h"

#include "textures/sprite-loader.h"

void BuildingTextures::loadHippodromeSpectators() {
    if(fHippodromeSpectatorsLoaded) return;
    fHippodromeSpectatorsLoaded = true;

    const auto& sds = eHippodromeSpectatorsSpriteData30;
    SpriteLoader loader(fTileH, "hippodromeSpectators", sds,
                         &ePoseidonImps2Offset, fRenderer);

    for(int j = 993; j < 1121;) {
        auto& coll = fHippodromeSpectators.emplace_back(fRenderer);
        for(int k = 0; k < 32; k++, j++) {
            loader.load(993, j, coll);
        }
    }
}

void BuildingTextures::loadHippodromeFeces() {
    if(fHippodromeFecesLoaded) return;
    fHippodromeFecesLoaded = true;

    const auto& sds = eHippodromeFecesSpriteData30;
    SpriteLoader loader(fTileH, "hippodromeFeces", sds,
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

void BuildingTextures::loadHippodromeFinish() {
    if(fHippodromeFinishLoaded) return;
    fHippodromeFinishLoaded = true;

    const auto& sds = eHippodromeFinishSpriteData30;
    SpriteLoader loader(fTileH, "hippodromeFinish", sds,
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
