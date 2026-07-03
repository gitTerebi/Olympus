#include "character-textures.h"

#include "sprite-loader.h"

#include "offsets/zeus_calydonianboar.h"
#include "offsets/zeus_cerberus.h"
#include "offsets/Poseidon_Chimera.h"
#include "offsets/zeus_cyclops.h"
#include "offsets/zeus_dragon.h"
#include "offsets/Poseidon_Echidna.h"
#include "offsets/Poseidon_Harpie.h"
#include "offsets/zeus_hector.h"

#include "spriteData/calydonianBoar30.h"

#include "spriteData/cerberus30.h"

#include "spriteData/chimera30.h"

#include "spriteData/cyclops30.h"

#include "spriteData/dragon30.h"

#include "spriteData/echidna30.h"

#include "spriteData/harpie30.h"

#include "spriteData/hector30.h"

void CharacterTextures::loadCalydonianBoar() {
    if(fCalydonianBoarLoaded) return;
    const auto& sds = eCalydonianBoarSpriteData30;
    fCalydonianBoarLoaded = true;
    SpriteLoader loader(fTileH, "calydonianBoar", sds,
                         &eZeus_calydonianboarOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fCalydonianBoar.fWalk);
    loader.loadSkipFlipped(1, 97, 201, fCalydonianBoar.fDie);
    loader.loadSkipFlipped(1, 201, 361, fCalydonianBoar.fFight);
    loader.loadSkipFlipped(1, 361, 457, fCalydonianBoar.fFight2);
}

void CharacterTextures::loadCerberus() {
    if(fCerberusLoaded) return;
    const auto& sds = eCerberusSpriteData30;
    fCerberusLoaded = true;
    SpriteLoader loader(fTileH, "cerberus", sds,
                         &eZeus_cerberusOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 185, fCerberus.fWalk);
    loader.loadSkipFlipped(1, 185, 361, fCerberus.fDie);
    loader.loadSkipFlipped(1, 361, 465, fCerberus.fFight);
    loader.loadSkipFlipped(1, 465, 633, fCerberus.fFight2);
}

void CharacterTextures::loadChimera() {
    if(fChimeraLoaded) return;
    const auto& sds = eChimeraSpriteData30;
    fChimeraLoaded = true;
    SpriteLoader loader(fTileH, "chimera", sds,
                         &ePoseidon_ChimeraOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 129, fChimera.fWalk);
    loader.loadSkipFlipped(1, 129, 289, fChimera.fDie);
    loader.loadSkipFlipped(1, 289, 489, fChimera.fFight);
    loader.loadSkipFlipped(1, 489, 641, fChimera.fFight2);
}

void CharacterTextures::loadCyclops() {
    if(fCyclopsLoaded) return;
    const auto& sds = eCyclopsSpriteData30;
    fCyclopsLoaded = true;
    SpriteLoader loader(fTileH, "cyclops", sds,
                         &eZeus_cyclopsOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 121, fCyclops.fWalk);
    loader.loadSkipFlipped(1, 121, 289, fCyclops.fDie);
    loader.loadSkipFlipped(1, 289, 425, fCyclops.fFight);
    loader.loadSkipFlipped(1, 425, 553, fCyclops.fFight2);
}

void CharacterTextures::loadDragon() {
    if(fDragonLoaded) return;
    const auto& sds = eDragonSpriteData30;
    fDragonLoaded = true;
    SpriteLoader loader(fTileH, "dragon", sds,
                         &eZeus_dragonOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 161, fDragon.fWalk);
    loader.loadSkipFlipped(1, 161,409, fDragon.fDie);
    loader.loadSkipFlipped(1, 409, 553, fDragon.fFight);
    loader.loadSkipFlipped(1, 553, 697, fDragon.fFight2);
}

void CharacterTextures::loadEchidna() {
    if(fEchidnaLoaded) return;
    const auto& sds = eEchidnaSpriteData30;
    fEchidnaLoaded = true;
    SpriteLoader loader(fTileH, "echidna", sds,
                         &ePoseidon_EchidnaOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 153, fEchidna.fWalk);
    loader.loadSkipFlipped(1, 153, 345, fEchidna.fDie);
    loader.loadSkipFlipped(1, 345, 513, fEchidna.fFight);
    loader.loadSkipFlipped(1, 513, 649, fEchidna.fFight2);
}

void CharacterTextures::loadHarpie() {
    if(fHarpieLoaded) return;
    const auto& sds = eHarpieSpriteData30;
    fHarpieLoaded = true;
    SpriteLoader loader(fTileH, "harpie", sds,
                         &ePoseidon_HarpieOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 337, fHarpies.fWalk);
    loader.loadSkipFlipped(1, 337, 617, fHarpies.fDie);
    loader.loadSkipFlipped(1, 617, 769, fHarpies.fFight);
    loader.loadSkipFlipped(1, 769, 929, fHarpies.fFight2);
}

void CharacterTextures::loadHector() {
    if(fHectorLoaded) return;
    const auto& sds = eHectorSpriteData30;
    fHectorLoaded = true;
    SpriteLoader loader(fTileH, "hector", sds,
                         &eZeus_hectorOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 121, fHector.fWalk);
    loader.loadSkipFlipped(1, 121, 201, fHector.fDie);
    loader.loadSkipFlipped(1, 201, 329, fHector.fFight);
    loader.loadSkipFlipped(1, 329, 465, fHector.fFight2);
}
