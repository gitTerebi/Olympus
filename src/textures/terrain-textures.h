#ifndef TERRAIN_TEXTURES_H
#define TERRAIN_TEXTURES_H

#include "texture-collection.h"

class TerrainTextures {
public:
    TerrainTextures(const int tileW, const int tileH,
                     SDL_Renderer* const renderer);

    void loadAll();

    void load();

    const int fTileW;
    const int fTileH;
    SDL_Renderer* const fRenderer;

    bool fPoseidonTreesLoaded = false;
    void loadPoseidonTrees();
    bool fBlackMarbleLoaded = false;
    void loadBlackMarble();
    bool fOrichalcLoaded = false;
    void loadOrichalc();

    TextureCollection fDryTerrainTexs;
    TextureCollection fBeachTerrainTexs;
    TextureCollection fBeachToDryTerrainTexs;
    TextureCollection fWaterTerrainTexs;
    std::vector<TextureCollection> fWaterTexs;
    std::vector<TextureCollection> fWaterToDryTerrainTexs;
    TextureCollection fDeepMarsh;
    TextureCollection fMarsh;
    std::vector<TextureCollection> fMarshToDry;
    TextureCollection fLava;
    std::vector<TextureCollection> fLavaToDry;
    TextureCollection fFertileTerrainTexs;
    TextureCollection fFertileToDryTerrainTexs;
    std::vector<TextureCollection> fDryToScrubTerrainTexs;
    TextureCollection fScrubTerrainTexs;
    std::vector<TextureCollection> fFertileToScrubTerrainTexs;
    std::vector<TextureCollection> fForestToDryTerrainTexs;
    TextureCollection fForestToScrubTerrainTexs;
    TextureCollection fForestTerrainTexs;
    std::vector<TextureCollection> fPoseidonForestToDryTerrainTexs;
    TextureCollection fPoseidonForestToScrubTerrainTexs;
    TextureCollection fPoseidonForestTerrainTexs;
    TextureCollection fChoppedForestToScrubTerrainTexs;
    TextureCollection fChoppedForestTerrainTexs;
    std::vector<TextureCollection> fWaterToBeachTerrainTexs;
    std::shared_ptr<Texture> fInvalidTex;
    std::shared_ptr<Texture> fSelectedTex;
    TextureCollection fWaterToBeachToDryTerrainTexs;

    TextureCollection fFlatStonesTerrainTexs;
    TextureCollection fLargeFlatStonesTerrainTexs;
    TextureCollection fHugeFlatStonesTerrainTexs;

    TextureCollection fBronzeTerrainTexs;
    TextureCollection fLargeBronzeTerrainTexs;
    TextureCollection fHugeBronzeTerrainTexs;

    TextureCollection fSilverTerrainTexs;
    TextureCollection fLargeSilverTerrainTexs;
    TextureCollection fHugeSilverTerrainTexs;

    TextureCollection fTallStoneTerrainTexs;
    TextureCollection fLargeTallStoneTerrainTexs;
    TextureCollection fHugeTallStoneTerrainTexs;

    TextureCollection fOrichalcTerrainTexs;
    TextureCollection fLargeOrichalcTerrainTexs;
    TextureCollection fHugeOrichalcTerrainTexs;

    TextureCollection fAppeal;
    TextureCollection fHouseAppeal;

    TextureCollection fBeachRoad;
    TextureCollection fToBeachRoad;
    TextureCollection fRoad;
    TextureCollection fPrettyRoad;

    TextureCollection fSanctuaryStairs;

    TextureCollection fTinyStones;

    TextureCollection fFlatMarble;
    TextureCollection fDryToMarble;
    TextureCollection fMarble;
    TextureCollection fDeepMarble;

    TextureCollection fFlatBlackMarble;
    TextureCollection fDryToBlackMarble;
    TextureCollection fBlackMarble;
    TextureCollection fDeepBlackMarble;

    std::shared_ptr<Texture> fBuildingBase;
    std::shared_ptr<Texture> fSelectedBuildingBase;

    TextureCollection fBuildingBase2;
    TextureCollection fBuildingBase3;

    TextureCollection fElevation;
    TextureCollection fDoubleElevation;
    TextureCollection fDoubleElevation2;
    TextureCollection fHalfElevation;
    TextureCollection fHalfElevation2;

    TextureCollection fQuakeTexs;
};

#endif // TERRAIN_TEXTURES_H
