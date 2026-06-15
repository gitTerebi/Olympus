#include "forest-to-dry.h"

#include "engine/etile.h"

ForestToDryId ForestToDry::get(eTile* const tile, const eWorldDirection dir) {
    if(!tile) return ForestToDryId::none;

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

    ForestToDryId result;
    if(tl || tr || bl || br || t || l || r || b) {
        result = ForestToDryId::somewhere;
    } else {
        result = ForestToDryId::none;
    }

    return result;
}
