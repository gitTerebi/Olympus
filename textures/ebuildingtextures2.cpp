#include "ebuildingtextures.h"

#include "spriteData/hippodromeFinish15.h"
#include "spriteData/hippodromeFinish30.h"
#include "spriteData/hippodromeFinish45.h"
#include "spriteData/hippodromeFinish60.h"

#include "spriteData/hippodromeSpectators15.h"
#include "spriteData/hippodromeSpectators30.h"
#include "spriteData/hippodromeSpectators45.h"
#include "spriteData/hippodromeSpectators60.h"

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
