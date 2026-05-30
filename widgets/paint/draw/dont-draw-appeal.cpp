#include "dont-draw-appeal.h"

bool dontDrawAppeal(const eTerrain terr)
{
    return terr == eTerrain::stones ||
           terr == eTerrain::flatStones ||
           terr == eTerrain::tallStones ||
           terr == eTerrain::copper ||
           terr == eTerrain::silver ||
           terr == eTerrain::orichalc ||
           terr == eTerrain::water;
}
