#ifndef EDESTRUCTIONTEXTURES_H
#define EDESTRUCTIONTEXTURES_H

#include "etexturecollection.h"

class eDestructionTextures {
public:
    eDestructionTextures(const int tileW, const int tileH,
                         SDL_Renderer* const renderer);

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    void loadAll();

    bool fFireLoaded = false;
    void loadFire();

    bool fBlessedLoaded = false;
    void loadBlessed();
    bool fCursedLoaded = false;
    void loadCursed();

    bool fRockLoaded = false;
    void loadRock();
    bool fBlackSpearLoaded = false;
    void loadBlackSpear();
    bool fSpearLoaded = false;
    void loadSpear();
    bool fBlackArrowLoaded = false;
    void loadBlackArrow();
    bool fArrowLoaded = false;
    void loadArrow();

    bool fBlessLoaded = false;
    void loadBless();
    bool fCurseLoaded = false;
    void loadCurse();

    bool fPlagueLoaded = false;
    void loadPlague();

    bool fGodOrangeMissileLoaded = false;
    void loadGodOrangeMissile();
    bool fGodBlueArrowLoaded = false;
    void loadGodBlueArrow();
    bool fGodOrangeArrowLoaded = false;
    void loadGodOrangeArrow();
    bool fMonsterMissileLoaded = false;
    void loadMonsterMissile();
    bool fGodBlueMissileLoaded = false;
    void loadGodBlueMissile();
    bool fGodRedMissileLoaded = false;
    void loadGodRedMissile();
    bool fGodGreenMissileLoaded = false;
    void loadGodGreenMissile();
    bool fGodPinkMissileLoaded = false;
    void loadGodPinkMissile();
    bool fGodPurpleMissileLoaded = false;
    void loadGodPurpleMissile();
    bool fOrichalcMissileLoaded = false;
    void loadOrichalcMissile();
    bool fWaveLoaded = false;
    void loadWave();
    bool fLavaLoaded = false;
    void loadLava();

    std::vector<eTextureCollection> fFire;
//    std::vector<eTextureCollection> fBigFire;

    eTextureCollection fBlessed;
    eTextureCollection fCursed;

    eTextureCollection fRock;
    eTextureCollection fBlackSpear;
    eTextureCollection fSpear;
    eTextureCollection fBlackArrow;
    eTextureCollection fArrow;

    std::vector<eTextureCollection> fBless;
    std::vector<eTextureCollection> fCurse;

    eTextureCollection fPlague;

    std::vector<eTextureCollection> fGodOrangeMissile;
    std::vector<eTextureCollection> fGodBlueArrow;
    std::vector<eTextureCollection> fGodOrangeArrow;
    std::vector<eTextureCollection> fMonsterMissile;
    std::vector<eTextureCollection> fGodBlueMissile;
    std::vector<eTextureCollection> fGodRedMissile;
    std::vector<eTextureCollection> fGodGreenMissile;
    std::vector<eTextureCollection> fGodPinkMissile;
    std::vector<eTextureCollection> fGodPurpleMissile;
    std::vector<eTextureCollection> fOrichalcMissile;

    eTextureCollection fWave;
    eTextureCollection fLava;
};

#endif // EDESTRUCTIONTEXTURES_H
