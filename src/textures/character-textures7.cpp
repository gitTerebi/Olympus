#include "character-textures.h"

#include "sprite-loader.h"

#include "spriteData/egyptianHoplite30.h"

#include "spriteData/egyptianArcher30.h"

#include "spriteData/egyptianChariot30.h"

#include "spriteData/atlanteanHoplite30.h"

#include "spriteData/atlanteanArcher30.h"

#include "spriteData/atlanteanChariot30.h"

#include "spriteData/mayanHoplite30.h"

#include "spriteData/mayanArcher30.h"

#include "spriteData/oceanidHoplite30.h"

#include "spriteData/oceanidSpearthrower30.h"

#include "spriteData/phoenicianHorseman30.h"

#include "spriteData/phoenicianArcher30.h"

#include "offsets/Poseidon_Egyptian.h"
#include "offsets/Poseidon_Atlantean.h"
#include "offsets/Poseidon_Mayan.h"
#include "offsets/Poseidon_Oceanid.h"
#include "offsets/Poseidon_Phoenician.h"

void CharacterTextures::loadAtlanteanChariot() {
    if(fAtlanteanChariotLoaded) return;
    fAtlanteanChariotLoaded = true;
    const auto& sds = eAtlanteanChariotSpriteData30;
    SpriteLoader loader(fTileH, "atlanteanChariot", sds,
                         &ePoseidon_AtlanteanOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fAtlanteanChariot.fWalk);
    loader.loadSkipFlipped(1, 169, 265, fAtlanteanChariot.fFight);
    loader.loadSkipFlipped(1, 97, 169, fAtlanteanChariot.fDie);
}

void CharacterTextures::loadAtlanteanArcher() {
    if(fAtlanteanArcherLoaded) return;
    fAtlanteanArcherLoaded = true;
    const auto& sds = eAtlanteanArcherSpriteData30;
    SpriteLoader loader(fTileH, "atlanteanArcher", sds,
                         &ePoseidon_AtlanteanOffset, fRenderer);

    loader.loadSkipFlipped(265, 265, 361, fAtlanteanArcher.fWalk);
    loader.loadSkipFlipped(265, 369, 489, fAtlanteanArcher.fFight);

    for(int i = 361; i < 369; i++) {
        loader.load(265, i, fAtlanteanArcher.fDie);
    }
}

void CharacterTextures::loadAtlanteanHoplite() {
    if(fAtlanteanHopliteLoaded) return;
    fAtlanteanHopliteLoaded = true;
    const auto& sds = eAtlanteanHopliteSpriteData30;
    SpriteLoader loader(fTileH, "atlanteanHoplite", sds,
                         &ePoseidon_AtlanteanOffset, fRenderer);

    loader.loadSkipFlipped(489, 489, 585, fAtlanteanHoplite.fWalk);
    loader.loadSkipFlipped(489, 593, 657, fAtlanteanHoplite.fFight);

    for(int i = 585; i < 593; i++) {
        loader.load(489, i, fAtlanteanHoplite.fDie);
    }
}

void CharacterTextures::loadEgyptianHoplite() {
    if(fEgyptianHopliteLoaded) return;
    fEgyptianHopliteLoaded = true;
    const auto& sds = eEgyptianHopliteSpriteData30;
    SpriteLoader loader(fTileH, "egyptianHoplite", sds,
                         &ePoseidon_EgyptianOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fEgyptianHoplite.fWalk);
    loader.loadSkipFlipped(1, 105, 201, fEgyptianHoplite.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fEgyptianHoplite.fDie);
    }
}

void CharacterTextures::loadEgyptianChariot() {
    if(fEgyptianChariotLoaded) return;
    fEgyptianChariotLoaded = true;
    const auto& sds = eEgyptianChariotSpriteData30;
    SpriteLoader loader(fTileH, "egyptianChariot", sds,
                         &ePoseidon_EgyptianOffset, fRenderer);

    loader.loadSkipFlipped(201, 201, 297, fEgyptianChariot.fWalk);
    loader.loadSkipFlipped(201, 369, 465, fEgyptianChariot.fFight);
    loader.loadSkipFlipped(201, 297, 369, fEgyptianChariot.fDie);
}

void CharacterTextures::loadEgyptianArcher() {
    if(fEgyptianArcherLoaded) return;
    fEgyptianArcherLoaded = true;
    const auto& sds = eEgyptianArcherSpriteData30;
    SpriteLoader loader(fTileH, "egyptianArcher", sds,
                         &ePoseidon_EgyptianOffset, fRenderer);

    loader.loadSkipFlipped(465, 465, 561, fEgyptianArcher.fWalk);
    loader.loadSkipFlipped(465, 569, 665, fEgyptianArcher.fFight);

    for(int i = 561; i < 569; i++) {
        loader.load(465, i, fEgyptianArcher.fDie);
    }
}

void CharacterTextures::loadMayanHoplite() {
    if(fMayanHopliteLoaded) return;
    fMayanHopliteLoaded = true;
    const auto& sds = eMayanHopliteSpriteData30;
    SpriteLoader loader(fTileH, "mayanHoplite", sds,
                         &ePoseidon_MayanOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fMayanHoplite.fWalk);
    loader.loadSkipFlipped(1, 105, 201, fMayanHoplite.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fMayanHoplite.fDie);
    }
}

void CharacterTextures::loadMayanArcher() {
    if(fMayanArcherLoaded) return;
    fMayanArcherLoaded = true;
    const auto& sds = eMayanArcherSpriteData30;
    SpriteLoader loader(fTileH, "mayanArcher", sds,
                         &ePoseidon_MayanOffset, fRenderer);

    loader.loadSkipFlipped(201, 201, 297, fMayanArcher.fWalk);
    loader.loadSkipFlipped(201, 305, 401, fMayanArcher.fFight);

    for(int i = 297; i < 305; i++) {
        loader.load(201, i, fMayanArcher.fDie);
    }
}

void CharacterTextures::loadPhoenicianHorseman() {
    if(fPhoenicianHorsemanLoaded) return;
    fPhoenicianHorsemanLoaded = true;
    const auto& sds = ePhoenicianHorsemanSpriteData30;
    SpriteLoader loader(fTileH, "phoenicianHorseman", sds,
                         &ePoseidon_PhoenicianOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fPhoenicianHorseman.fWalk);
    loader.loadSkipFlipped(1, 105, 201, fPhoenicianHorseman.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fPhoenicianHorseman.fDie);
    }
}

void CharacterTextures::loadPhoenicianArcher() {
    if(fPhoenicianArcherLoaded) return;
    fPhoenicianArcherLoaded = true;
    const auto& sds = ePhoenicianArcherSpriteData30;
    SpriteLoader loader(fTileH, "phoenicianArcher", sds,
                         &ePoseidon_PhoenicianOffset, fRenderer);

    loader.loadSkipFlipped(201, 201, 297, fPhoenicianArcher.fWalk);
    loader.loadSkipFlipped(201, 305, 401, fPhoenicianArcher.fFight);

    for(int i = 297; i < 305; i++) {
        loader.load(201, i, fPhoenicianArcher.fDie);
    }
}

void CharacterTextures::loadOceanidHoplite() {
    if(fOceanidHopliteLoaded) return;
    fOceanidHopliteLoaded = true;
    const auto& sds = eOceanidHopliteSpriteData30;
    SpriteLoader loader(fTileH, "oceanidHoplite", sds,
                         &ePoseidon_OceanidOffset, fRenderer);

    loader.loadSkipFlipped(201, 201, 297, fOceanidHoplite.fWalk);
    loader.loadSkipFlipped(201, 305, 369, fOceanidHoplite.fFight);

    for(int i = 297; i < 305; i++) {
        loader.load(201, i, fOceanidHoplite.fDie);
    }
}

void CharacterTextures::loadOceanidSpearthrower() {
    if(fOceanidSpearthrowerLoaded) return;
    fOceanidSpearthrowerLoaded = true;
    const auto& sds = eOceanidSpearthrowerSpriteData30;
    SpriteLoader loader(fTileH, "oceanidSpearthrower", sds,
                         &ePoseidon_OceanidOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 97, fOceanidSpearthrower.fWalk);
    loader.loadSkipFlipped(1, 105, 201, fOceanidSpearthrower.fFight);

    for(int i = 97; i < 105; i++) {
        loader.load(1, i, fOceanidSpearthrower.fDie);
    }
}

