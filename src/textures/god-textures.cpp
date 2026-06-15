#include "god-textures.h"

#include "offsets/Zeus_Aphrodite.h"
#include "offsets/Zeus_Apollo.h"
#include "offsets/Zeus_Ares.h"
#include "offsets/Zeus_Artemis.h"
#include "offsets/Zeus_Athena.h"
#include "offsets/Poseidon_Atlas.h"
#include "offsets/Zeus_Demeter.h"
#include "offsets/Zeus_Dionysus.h"
#include "offsets/Zeus_Hades.h"
#include "offsets/Zeus_Hephaestus.h"
#include "offsets/Poseidon_Hera.h"
#include "offsets/Zeus_Hermes.h"
#include "offsets/Zeus_Poseidon.h"
#include "offsets/Zeus_Zeus.h"

#include "spriteData/aphrodite15.h"
#include "spriteData/aphrodite30.h"
#include "spriteData/aphrodite45.h"
#include "spriteData/aphrodite60.h"

#include "spriteData/apollo15.h"
#include "spriteData/apollo30.h"
#include "spriteData/apollo45.h"
#include "spriteData/apollo60.h"

#include "spriteData/ares15.h"
#include "spriteData/ares30.h"
#include "spriteData/ares45.h"
#include "spriteData/ares60.h"

#include "spriteData/artemis15.h"
#include "spriteData/artemis30.h"
#include "spriteData/artemis45.h"
#include "spriteData/artemis60.h"

#include "spriteData/athena15.h"
#include "spriteData/athena30.h"
#include "spriteData/athena45.h"
#include "spriteData/athena60.h"

#include "spriteData/atlas15.h"
#include "spriteData/atlas30.h"
#include "spriteData/atlas45.h"
#include "spriteData/atlas60.h"

#include "spriteData/demeter15.h"
#include "spriteData/demeter30.h"
#include "spriteData/demeter45.h"
#include "spriteData/demeter60.h"

#include "spriteData/dionysus15.h"
#include "spriteData/dionysus30.h"
#include "spriteData/dionysus45.h"
#include "spriteData/dionysus60.h"

#include "spriteData/hades15.h"
#include "spriteData/hades30.h"
#include "spriteData/hades45.h"
#include "spriteData/hades60.h"

#include "spriteData/hephaestus15.h"
#include "spriteData/hephaestus30.h"
#include "spriteData/hephaestus45.h"
#include "spriteData/hephaestus60.h"

#include "spriteData/hera15.h"
#include "spriteData/hera30.h"
#include "spriteData/hera45.h"
#include "spriteData/hera60.h"

#include "spriteData/hermes15.h"
#include "spriteData/hermes30.h"
#include "spriteData/hermes45.h"
#include "spriteData/hermes60.h"

#include "spriteData/poseidon15.h"
#include "spriteData/poseidon30.h"
#include "spriteData/poseidon45.h"
#include "spriteData/poseidon60.h"

#include "spriteData/zeus15.h"
#include "spriteData/zeus30.h"
#include "spriteData/zeus45.h"
#include "spriteData/zeus60.h"

#include "sprite-loader.h"

GodTextures::GodTextures(const int tileW, const int tileH,
                           SDL_Renderer* const renderer) :
    fTileW(tileW), fTileH(tileH),
    fRenderer(renderer),

    fAphrodite(renderer),
    fApollo(renderer),
    fAres(renderer),
    fArtemis(renderer),
    fAthena(renderer),
    fAtlas(renderer),
    fDemeter(renderer),
    fDionysus(renderer),
    fHades(renderer),
    fHephaestus(renderer),
    fHera(renderer),
    fHermes(renderer),
    fPoseidon(renderer),
    fZeus(renderer) {

}

void GodTextures::loadAll() {
    loadAphrodite();
    loadApollo();
    loadAres();
    loadArtemis();
    loadAthena();
    loadAtlas();
    loadDemeter();
    loadDionysus();
    loadHades();
    loadHephaestus();
    loadHera();
    loadHermes();
    loadPoseidon();
    loadZeus();
}

void loadBasicGod(BasicGodTextures& god,
                  const int w0, const int w1,
                  const int d0, const int d1,
                  const int f0, const int f1,
                  SpriteLoader& loader) {
    loader.loadSkipFlipped(w0, w0, w1, god.fWalk);
    loader.loadSkipFlipped(w0, f0, f1, god.fFight);

    for(int i = d0; i < d1; i++) {
        loader.load(w0, i, god.fDisappear);
    }
}

void loadExtendedGod(ExtendedGodTextures& god,
                     const int w0, const int w1,
                     const int d0, const int d1,
                     const int f0, const int f1,
                     const int b0, const int b1,
                     SpriteLoader& loader) {
    loadBasicGod(god, w0, w1, d0, d1, f0, f1, loader);
    loader.loadSkipFlipped(w0, b0, b1, god.fBless);
}

void loadDionysusTextures(DionysusTextures& god,
                          const int w0, const int w1,
                          const int d0, const int d1,
                          const int f0, const int f1,
                          const int a0, const int a1,
                          SpriteLoader& loader) {
    loadBasicGod(god, w0, w1, d0, d1, f0, f1, loader);
    for(int i = a0; i < a1; i++) {
        loader.load(w0, i, god.fAppear);
    }
}

void loadHermesTextures(HermesTextures& god,
                        const int w0, const int w1,
                        const int r0, const int r1,
                        const int f0, const int f1,
                        SpriteLoader& loader) {
    loadBasicGod(god, w0, w1, 0, 0, f0, f1, loader);
    loader.loadSkipFlipped(w0, r0, r1, god.fRun);
}

void GodTextures::loadAphrodite() {
    if(fAphroditeLoaded) return;
    fAphroditeLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eAphroditeSpriteData15,
                                 eAphroditeSpriteData30,
                                 eAphroditeSpriteData45,
                                 eAphroditeSpriteData60);
    SpriteLoader loader(fTileH, "aphrodite", sds,
                         &eZeus_AphroditeOffset, fRenderer);
    loadBasicGod(fAphrodite, 1, 209, 209, 241, 241, 441, loader);
}

void GodTextures::loadApollo() {
    if(fApolloLoaded) return;
    fApolloLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eApolloSpriteData15,
                                 eApolloSpriteData30,
                                 eApolloSpriteData45,
                                 eApolloSpriteData60);
    SpriteLoader loader(fTileH, "apollo", sds,
                         &eZeus_ApolloOffset, fRenderer);
    loadExtendedGod(fApollo, 1, 185, 185, 201, 202, 346, 347, 515, loader);
}

void GodTextures::loadAres() {
    if(fAresLoaded) return;
    fAresLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eAresSpriteData15,
                                 eAresSpriteData30,
                                 eAresSpriteData45,
                                 eAresSpriteData60);
    SpriteLoader loader(fTileH, "ares", sds,
                         &eZeus_AresOffset, fRenderer);
    loadBasicGod(fAres, 1, 249, 249, 265, 265, 433, loader);
}

void GodTextures::loadArtemis() {
    if(fArtemisLoaded) return;
    fArtemisLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eArtemisSpriteData15,
                                 eArtemisSpriteData30,
                                 eArtemisSpriteData45,
                                 eArtemisSpriteData60);
    SpriteLoader loader(fTileH, "artemis", sds,
                         &eZeus_ArtemisOffset, fRenderer);//wdfb
    loadExtendedGod(fArtemis, 1, 105, 105, 122, 122, 266, 267, 403, loader);
}

void GodTextures::loadAthena() {
    if(fAthenaLoaded) return;
    fAthenaLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eAthenaSpriteData15,
                                 eAthenaSpriteData30,
                                 eAthenaSpriteData45,
                                 eAthenaSpriteData60);
    SpriteLoader loader(fTileH, "athena", sds,
                         &eZeus_AthenaOffset, fRenderer);
    loadExtendedGod(fAthena, 1, 121, 121, 137, 137, 297, 298, 426, loader);
}

void GodTextures::loadAtlas() {
    if(fAtlasLoaded) return;
    fAtlasLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eAtlasSpriteData15,
                                 eAtlasSpriteData30,
                                 eAtlasSpriteData45,
                                 eAtlasSpriteData60);
    SpriteLoader loader(fTileH, "atlas", sds,
                         &ePoseidon_AtlasOffset, fRenderer);
    loadExtendedGod(fAtlas, 1, 161, 161, 195, 195, 355, 356, 604, loader);
}

void GodTextures::loadDemeter() {
    if(fDemeterLoaded) return;
    fDemeterLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eDemeterSpriteData15,
                                 eDemeterSpriteData30,
                                 eDemeterSpriteData45,
                                 eDemeterSpriteData60);
    SpriteLoader loader(fTileH, "demeter", sds,
                         &eZeus_DemeterOffset, fRenderer);
    loadBasicGod(fDemeter, 1, 225, 225, 243, 243, 507, loader);
}

void GodTextures::loadDionysus() {
    if(fDionysusLoaded) return;
    fDionysusLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eDionysusSpriteData15,
                                 eDionysusSpriteData30,
                                 eDionysusSpriteData45,
                                 eDionysusSpriteData60);
    SpriteLoader loader(fTileH, "dionysus", sds,
                         &eZeus_DionysusOffset, fRenderer);
    loadDionysusTextures(fDionysus, 1, 193, 193, 212, 212, 476, 476, 498, loader);
}

void GodTextures::loadHades() {
    if(fHadesLoaded) return;
    fHadesLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHadesSpriteData15,
                                 eHadesSpriteData30,
                                 eHadesSpriteData45,
                                 eHadesSpriteData60);
    SpriteLoader loader(fTileH, "hades", sds,
                         &eZeus_HadesOffset, fRenderer);
    loadBasicGod(fHades, 1, 185, 185, 217, 217, 345, loader);
}

void GodTextures::loadHephaestus() {
    if(fHephaestusLoaded) return;
    fHephaestusLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHephaestusSpriteData15,
                                 eHephaestusSpriteData30,
                                 eHephaestusSpriteData45,
                                 eHephaestusSpriteData60);
    SpriteLoader loader(fTileH, "hephaestus", sds,
                         &eZeus_HephaestusOffset, fRenderer);
    loadBasicGod(fHephaestus, 1, 137, 137, 171, 171, 307, loader);
}

void GodTextures::loadHera() {
    if(fHeraLoaded) return;
    fHeraLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHeraSpriteData15,
                                 eHeraSpriteData30,
                                 eHeraSpriteData45,
                                 eHeraSpriteData60);
    SpriteLoader loader(fTileH, "hera", sds,
                         &ePoseidon_HeraOffset, fRenderer);
    loadBasicGod(fHera, 1, 177, 177, 208, 208, 456, loader);
}

void GodTextures::loadHermes() {
    if(fHermesLoaded) return;
    fHermesLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eHermesSpriteData15,
                                 eHermesSpriteData30,
                                 eHermesSpriteData45,
                                 eHermesSpriteData60);
    SpriteLoader loader(fTileH, "hermes", sds,
                         &eZeus_HermesOffset, fRenderer);
    loadHermesTextures(fHermes, 1, 185, 185, 369, 369, 537, loader);
}

void GodTextures::loadPoseidon() {
    if(fPoseidonLoaded) return;
    fPoseidonLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 ePoseidonSpriteData15,
                                 ePoseidonSpriteData30,
                                 ePoseidonSpriteData45,
                                 ePoseidonSpriteData60);
    SpriteLoader loader(fTileH, "poseidon", sds,
                         &eZeus_PoseidonOffset, fRenderer);
    loadBasicGod(fPoseidon, 1, 137, 137, 170, 170, 418, loader);
}

void GodTextures::loadZeus() {
    if(fZeusLoaded) return;
    fZeusLoaded = true;

    const auto& sds = spriteData(fTileH,
                                 eZeusSpriteData15,
                                 eZeusSpriteData30,
                                 eZeusSpriteData45,
                                 eZeusSpriteData60);
    SpriteLoader loader(fTileH, "zeus", sds,
                         &eZeus_ZeusOffset, fRenderer);
    loadExtendedGod(fZeus, 1, 185, 185, 225, 226, 394, 395, 651, loader);
}
