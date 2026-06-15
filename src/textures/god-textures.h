#ifndef GOD_TEXTURES_H
#define GOD_TEXTURES_H

#include "etexturecollection.h"

struct BasicGodTextures {
    BasicGodTextures(SDL_Renderer* const renderer) :
        fDisappear(renderer) {};

    std::vector<eTextureCollection> fWalk;
    eTextureCollection fDisappear;
    std::vector<eTextureCollection> fFight;
};

struct DionysusTextures : public BasicGodTextures {
    DionysusTextures(SDL_Renderer* const renderer) :
        BasicGodTextures(renderer),
        fAppear(renderer) {};

    eTextureCollection fAppear;
};

struct HermesTextures : public BasicGodTextures {
    HermesTextures(SDL_Renderer* const renderer) :
        BasicGodTextures(renderer) {};

    std::vector<eTextureCollection> fRun;
};

struct ExtendedGodTextures : public BasicGodTextures {
    ExtendedGodTextures(SDL_Renderer* const renderer) :
        BasicGodTextures(renderer) {};


    std::vector<eTextureCollection> fBless;
};

class GodTextures {
public:
    GodTextures(const int tileW, const int tileH,
                 SDL_Renderer* const renderer);

    static const int sAphroditeDTime = 241 - 209; // disappear
    static const int sAphroditeFBTime = (441 - 241)/8; // fight / bless

    static const int sApolloDTime = 202 - 185;
    static const int sApolloFTime = (346 - 202)/8;
    static const int sApolloBTime = (515 - 347)/8; // bless

    static const int sAresDTime = 265 - 249;
    static const int sAresFBTime = (433 - 265)/8;

    static const int sArtemisDTime = 122 - 105;
    static const int sArtemisFTime = (266 - 122)/8;
    static const int sArtemisBTime = (403 - 267)/8;

    static const int sAthenaDTime = 137 - 121;
    static const int sAthenaFTime = (297 - 137)/8;
    static const int sAthenaBTime = (426 - 298)/8;

    static const int sAtlasDTime = 195 - 161;
    static const int sAtlasFTime = (355 - 195)/8;
    static const int sAtlasBTime = (604 - 356)/8;

    static const int sDemeterDTime = 243 - 225;
    static const int sDemeterFBTime = (507 - 243)/8;

    static const int sDionysusDTime = 212 - 193; // die
    static const int sDionysusFBTime = (476 - 212)/8;
    static const int sDionysusATime = 498 - 476; // appear

    static const int sHadesDTime = 217 - 185;
    static const int sHadesFBTime = (345 - 217)/8;

    static const int sHephaestusDTime = 171 - 137;
    static const int sHephaestusFBTime = (307 - 171)/8;

    static const int sHeraDTime = 208 - 177;
    static const int sHeraFBTime = (456 - 208)/8;

    static const int sHermesFBTime = (537 - 369)/8;

    static const int sPoseidonDTime = 170 - 137;
    static const int sPoseidonFBTime = (418 - 170)/8;

    static const int sZeusDTime = 225 - 185;
    static const int sZeusFTime = (394 - 226)/8;
    static const int sZeusBTime = (651 - 395)/8;

    void loadAll();

    bool fAphroditeLoaded = false;
    void loadAphrodite();
    bool fApolloLoaded = false;
    void loadApollo();
    bool fAresLoaded = false;
    void loadAres();
    bool fArtemisLoaded = false;
    void loadArtemis();
    bool fAthenaLoaded = false;
    void loadAthena();
    bool fAtlasLoaded = false;
    void loadAtlas();
    bool fDemeterLoaded = false;
    void loadDemeter();
    bool fDionysusLoaded = false;
    void loadDionysus();
    bool fHadesLoaded = false;
    void loadHades();
    bool fHephaestusLoaded = false;
    void loadHephaestus();
    bool fHeraLoaded = false;
    void loadHera();
    bool fHermesLoaded = false;
    void loadHermes();
    bool fPoseidonLoaded = false;
    void loadPoseidon();
    bool fZeusLoaded = false;
    void loadZeus();

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    BasicGodTextures fAphrodite;
    ExtendedGodTextures fApollo;
    BasicGodTextures fAres;
    ExtendedGodTextures fArtemis;
    ExtendedGodTextures fAthena;
    ExtendedGodTextures fAtlas;
    BasicGodTextures fDemeter;
    DionysusTextures fDionysus;
    BasicGodTextures fHades;
    BasicGodTextures fHephaestus;
    BasicGodTextures fHera;
    HermesTextures fHermes;
    BasicGodTextures fPoseidon;
    ExtendedGodTextures fZeus;
};

#endif // GOD_TEXTURES_H
