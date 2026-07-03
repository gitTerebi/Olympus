#include "character-textures.h"

#include "sprite-loader.h"

#include "offsets/zeus_jason.h"
#include "offsets/zeus_odysseus.h"
#include "offsets/zeus_perseus.h"
#include "offsets/zeus_theseus.h"

#include "spriteData/jason30.h"

#include "spriteData/odysseus30.h"

#include "spriteData/perseus30.h"

#include "spriteData/theseus30.h"

void CharacterTextures::loadJason() {
    if(fJasonLoaded) return;
    const auto& sds = eJasonSpriteData30;
    fJasonLoaded = true;
    SpriteLoader loader(fTileH, "jason", sds,
                         &eZeus_jasonOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 249, fJason.fWalk);
    loader.loadSkipFlipped(1, 249, 409, fJason.fDie);
    loader.loadSkipFlipped(1, 409, 657, fJason.fFight);
}

void CharacterTextures::loadOdysseus() {
    if(fOdysseusLoaded) return;
    const auto& sds = eOdysseusSpriteData30;
    fOdysseusLoaded = true;
    SpriteLoader loader(fTileH, "odysseus", sds,
                         &eZeus_odysseusOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 241, fOdysseus.fWalk);
    loader.loadSkipFlipped(1, 241, 489, fOdysseus.fDie);
    loader.loadSkipFlipped(1, 489, 769, fOdysseus.fFight);
}

void CharacterTextures::loadPerseus() {
    if(fPerseusLoaded) return;
    const auto& sds = ePerseusSpriteData30;
    fPerseusLoaded = true;
    SpriteLoader loader(fTileH, "perseus", sds,
                         &eZeus_perseusOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 129, fPerseus.fWalk);
    loader.loadSkipFlipped(1, 129, 289, fPerseus.fDie);
    loader.loadSkipFlipped(1, 289, 417, fPerseus.fFight);
}

void CharacterTextures::loadTheseus() {
    if(fTheseusLoaded) return;
    const auto& sds = eTheseusSpriteData30;
    fTheseusLoaded = true;
    SpriteLoader loader(fTileH, "theseus", sds,
                         &eZeus_theseusOffset, fRenderer);

    loader.loadSkipFlipped(1, 1, 185, fTheseus.fWalk);
    loader.loadSkipFlipped(1, 185, 409, fTheseus.fDie);
    loader.loadSkipFlipped(1, 409, 585, fTheseus.fFight);
}
