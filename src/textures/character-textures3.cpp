#include "character-textures.h"

#include "sprite-loader.h"

#include "offsets/zeus_achilles.h"
#include "offsets/Poseidon_Atalanta.h"
#include "offsets/Poseidon_Bellerophon.h"
#include "offsets/zeus_heracles.h"

#include "spriteData/achilles30.h"

#include "spriteData/atalanta30.h"

#include "spriteData/bellerophon30.h"

#include "spriteData/heracles30.h"

void CharacterTextures::loadAchilles() {
    if(fAchillesLoaded) return;
    const auto& sds = eAchillesSpriteData30;
    fAchillesLoaded = true;
    SpriteLoader loader(fTileH, "achilles", sds,
                         &eZeus_achillesOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 257, fAchilles.fWalk);
    loader.loadSkipFlipped(1, 257, 457, fAchilles.fDie);
    loader.loadSkipFlipped(1, 457, 577, fAchilles.fFight);
}

void CharacterTextures::loadAtalanta() {
    if(fAtalantaLoaded) return;
    const auto& sds = eAtalantaSpriteData30;
    fAtalantaLoaded = true;
    SpriteLoader loader(fTileH, "atalanta", sds,
                         &ePoseidon_AtalantaOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 161, fAtalanta.fWalk);
    loader.loadSkipFlipped(1, 161, 329, fAtalanta.fDie);
    loader.loadSkipFlipped(1, 329, 473, fAtalanta.fFight);
}

void CharacterTextures::loadBellerophon() {
    if(fBellerophonLoaded) return;
    const auto& sds = eBellerophonSpriteData30;
    fBellerophonLoaded = true;
    SpriteLoader loader(fTileH, "bellerophon", sds,
                         &ePoseidon_BellerophonOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 121, fBellerophon.fWalk);
    loader.loadSkipFlipped(1, 121, 289, fBellerophon.fDie);
    loader.loadSkipFlipped(1, 289, 529, fBellerophon.fFight);
}

void CharacterTextures::loadHeracles() {
    if(fHeraclesLoaded) return;
    const auto& sds = eHeraclesSpriteData30;
    fHeraclesLoaded = true;
    SpriteLoader loader(fTileH, "heracles", sds,
                         &eZeus_heraclesOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 185, fHercules.fWalk);
    loader.loadSkipFlipped(1, 185, 345, fHercules.fDie);
    loader.loadSkipFlipped(1, 345, 481, fHercules.fFight);
}
