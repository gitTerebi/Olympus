#ifndef DESTRUCTION_TEXTURES_H
#define DESTRUCTION_TEXTURES_H

#include "texture-collection.h"

class DestructionTextures {
public:
    DestructionTextures(const int tileW, const int tileH,
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
    bool fDustLoaded = false;
    void loadDust();

    std::vector<TextureCollection> fFire;
//    std::vector<TextureCollection> fBigFire;

    TextureCollection fBlessed;
    TextureCollection fCursed;

    TextureCollection fRock;
    TextureCollection fBlackSpear;
    TextureCollection fSpear;
    TextureCollection fBlackArrow;
    TextureCollection fArrow;

    std::vector<TextureCollection> fBless;
    std::vector<TextureCollection> fCurse;

    TextureCollection fPlague;

    std::vector<TextureCollection> fGodOrangeMissile;
    std::vector<TextureCollection> fGodBlueArrow;
    std::vector<TextureCollection> fGodOrangeArrow;
    std::vector<TextureCollection> fMonsterMissile;
    std::vector<TextureCollection> fGodBlueMissile;
    std::vector<TextureCollection> fGodRedMissile;
    std::vector<TextureCollection> fGodGreenMissile;
    std::vector<TextureCollection> fGodPinkMissile;
    std::vector<TextureCollection> fGodPurpleMissile;
    std::vector<TextureCollection> fOrichalcMissile;

    TextureCollection fWave;
    std::vector<TextureCollection> fLava;
    std::vector<TextureCollection> fDust;
};

#endif // DESTRUCTION_TEXTURES_H
