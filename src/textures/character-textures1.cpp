#include "character-textures.h"

#include "sprite-loader.h"

#include "offsets/PoseidonImps.h"

#include "spriteData/deerHunter30.h"

#include "spriteData/deer30.h"

#include "spriteData/orangesCart30.h"

#include "spriteData/orichalcCart30.h"

#include "spriteData/orangeTender30.h"

#include "spriteData/blackMarbleTrailer30.h"

void CharacterTextures::loadBlackMarbleTrailer() {
    if(fBlackMarbleTrailerLoaded) return;
    fBlackMarbleTrailerLoaded = true;
    const auto& sds = eBlackMarbleTrailerSpriteData30;
    SpriteLoader loader(fTileH, "blackMarbleTrailer", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadTrailer(1019, 1019, 1027, fBlackMarbleBigTrailer, -4);
    loader.loadTrailer(1019, 1027, 1035, fBlackMarbleTrailer1, -7);
    loader.loadTrailer(1019, 1035, 1043, fBlackMarbleTrailer2, -7);
}

void CharacterTextures::loadOrangeTender() {
    if(fOrangeTenderLoded) return;
    fOrangeTenderLoded = true;
    const auto& sds = eOrangeTenderSpriteData30;
    SpriteLoader loader(fTileH, "orangeTender", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(1211, 1211, 1307, fOrangeTender.fWalk);
    loader.loadSkipFlipped(1211, 1315, 1411, fOrangeTender.fWorkOnTree);
    loader.loadSkipFlipped(1211, 1411, 1507, fOrangeTender.fCollect);

    for(int i = 1307; i < 1315; i++) {
        loader.load(1211, i, fOrangeTender.fDie);
    }
}

void CharacterTextures::loadOrangesCart() {
    if(fOrangesCartLoaded) return;
    fOrangesCartLoaded = true;
    const auto& sds = eOrangesCartSpriteData30;
    SpriteLoader loader(fTileH, "orangesCart", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(1091, 1091, 1115, fOrangesCart);
}

void CharacterTextures::loadOrichalcCart() {
    if(fOrichalcCartLoaded) return;
    fOrichalcCartLoaded = true;
    const auto& sds = eOrichalcCartSpriteData30;
    SpriteLoader loader(fTileH, "orichalcCart", sds,
                         &ePoseidonImpsOffset, fRenderer);

    loader.loadSkipFlipped(1115, 1115, 1131, fOrichalcCart);
}

void CharacterTextures::loadDeerHunter() {
    if(fDeerHunterLoaded) return;
    fDeerHunterLoaded = true;
    const auto& sds = eDeerHunterSpriteData30;

    SpriteLoader loader(fTileH, "deerHunter", sds,
                         &ePoseidonImpsOffset, fRenderer);
    loader.loadSkipFlipped(183, 183, 279, fDeerHunter.fWalk);
    loader.loadSkipFlipped(183, 287, 407, fDeerHunter.fCollect);
    loader.loadSkipFlipped(183, 435, 531, fDeerHunter.fCarry);

    for(int i = 279; i < 287; i++) {
        loader.load(183, i, fDeerHunter.fDie);
    }
}

void CharacterTextures::loadDeer() {
    if(fDeerLoaded) return;
    fDeerLoaded = true;

    const auto& sds = eDeerSpriteData30;
    SpriteLoader loader(fTileH, "deer", sds,
                         &ePoseidonImpsOffset, fRenderer);
    loader.loadSkipFlipped(531, 531, 627, fDeer.fWalk);
    loader.loadSkipFlipped(531, 635, 859, fDeer.fFight);
    loader.loadSkipFlipped(531, 859, 955, fDeer.fLayDown);

    for(int i = 627; i < 635; i++) {
        loader.load(531, i, fDeer.fDie);
    }
}
