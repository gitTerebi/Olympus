#include "fertile-to-dry.h"

#include "engine/etile.h"

FertileToDryId FertileToDry::get(eTile* const tile,
                                   const eWorldDirection dir) {
    if(!tile) return FertileToDryId::none;

    const auto terr = tile->terrain();

    bool tl;
    bool tr;
    bool br;
    bool bl;
    bool t;
    bool r;
    bool b;
    bool l;
    tile->neighboursWithTerrain(terr, tl, tr, br, bl, t, r, b, l, dir);

    FertileToDryId result;
    if(tl || tr || bl || br || t || l || r || b) {
        result = FertileToDryId::somewhere;
    } else {
        result = FertileToDryId::none;
    }

    return result;
}
