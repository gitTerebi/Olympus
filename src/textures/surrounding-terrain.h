#ifndef SURROUNDING_TERRAIN_H
#define SURROUNDING_TERRAIN_H

#include "engine/etile.h"

struct SurroundingTerrain {
    SurroundingTerrain(eTile* const tile,
                        const eWorldDirection dir);

    bool operator()(const eTerrain t,
                    const eTerrain tr,
                    const eTerrain r,
                    const eTerrain br,
                    const eTerrain b,
                    const eTerrain bl,
                    const eTerrain l,
                    const eTerrain tl);
private:
    eTerrain mTL;
    eTerrain mTR;
    eTerrain mBR;
    eTerrain mBL;

    eTerrain mT;
    eTerrain mR;
    eTerrain mB;
    eTerrain mL;
};

#endif // SURROUNDING_TERRAIN_H
