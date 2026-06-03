#ifndef ESANCTUARYBLUEPRINT_H
#define ESANCTUARYBLUEPRINT_H

#include <vector>

#include "buildings/ebuilding.h"

enum class eSanctEleType {
    none,
    stairs,
    tile,
    monument,
    altar,
    sanctuary,
    copper,
    silver,
    oliveTree,
    vine,
    orangeTree,

    woman,

    defaultStatue,
    aphroditeStatue,
    apolloStatue,
    aresStatue,
    artemisStatue,
    athenaStatue,
    atlasStatue,
    demeterStatue,
    dionysusStatue,
    hadesStatue,
    hephaestusStatue,
    heraStatue,
    hermesStatue,
    poseidonStatue,
    zeusStatue,
};

struct eSanctEle {
    int fX;
    int fY;
    int fA;
    int fId;
    eSanctEleType fType;
    bool fWarrior = false;
};

eSanctEle sanctEleFlip180(eSanctEle ele, int bpW, int bpH);

struct eSanctBlueprint {
    int fW;
    int fH;
    std::vector<std::vector<eSanctEle>> fTiles;
};

class eSanctBlueprints {
public:
    static eSanctBlueprints instance;

    static const eSanctBlueprint* sSanctuaryBlueprint(
            const eBuildingType type, const int rotateId);

    static void load();
    static bool loaded();

    eSanctBlueprint fZeusW;
    eSanctBlueprint fZeusH;
    eSanctBlueprint fZeusW2;
    eSanctBlueprint fZeusH2;

    eSanctBlueprint fAresW;
    eSanctBlueprint fAresH;
    eSanctBlueprint fAresW2;
    eSanctBlueprint fAresH2;

    eSanctBlueprint fAphroditeW;
    eSanctBlueprint fAphroditeH;
    eSanctBlueprint fAphroditeW2;
    eSanctBlueprint fAphroditeH2;

    eSanctBlueprint fApolloW;
    eSanctBlueprint fApolloH;
    eSanctBlueprint fApolloW2;
    eSanctBlueprint fApolloH2;

    eSanctBlueprint fAthenaW;
    eSanctBlueprint fAthenaH;
    eSanctBlueprint fAthenaW2;
    eSanctBlueprint fAthenaH2;

    eSanctBlueprint fAtlasW;
    eSanctBlueprint fAtlasH;
    eSanctBlueprint fAtlasW2;
    eSanctBlueprint fAtlasH2;

    eSanctBlueprint fArtemisW;
    eSanctBlueprint fArtemisH;
    eSanctBlueprint fArtemisW2;
    eSanctBlueprint fArtemisH2;

    eSanctBlueprint fDemeterW;
    eSanctBlueprint fDemeterH;
    eSanctBlueprint fDemeterW2;
    eSanctBlueprint fDemeterH2;

    eSanctBlueprint fDionysusW;
    eSanctBlueprint fDionysusH;
    eSanctBlueprint fDionysusW2;
    eSanctBlueprint fDionysusH2;

    eSanctBlueprint fHadesW;
    eSanctBlueprint fHadesH;
    eSanctBlueprint fHadesW2;
    eSanctBlueprint fHadesH2;

    eSanctBlueprint fHephaestusW;
    eSanctBlueprint fHephaestusH;
    eSanctBlueprint fHephaestusW2;
    eSanctBlueprint fHephaestusH2;

    eSanctBlueprint fHeraW;
    eSanctBlueprint fHeraH;
    eSanctBlueprint fHeraW2;
    eSanctBlueprint fHeraH2;

    eSanctBlueprint fHermesW;
    eSanctBlueprint fHermesH;
    eSanctBlueprint fHermesW2;
    eSanctBlueprint fHermesH2;

    eSanctBlueprint fPoseidonW;
    eSanctBlueprint fPoseidonH;
    eSanctBlueprint fPoseidonW2;
    eSanctBlueprint fPoseidonH2;
private:
    void loadImpl();

    bool mLoaded = false;
};

#endif // ESANCTUARYBLUEPRINT_H
