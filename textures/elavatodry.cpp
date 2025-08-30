#include "elavatodry.h"

#include "esurroundingterrain.h"

int eLavaToDry::get(eTile* const tile, const eWorldDirection dir) {
    const auto x = eTerrain::all ^ eTerrain::lava;
    const auto lava = eTerrain::lava;

    if(tile->terrain() != lava) return -1;

    eSurroundingTerrain tt(tile, dir);

    int id = -1;

    if(tt(lava, lava, lava, lava, x | lava, x, x | lava, lava)) {
        id = 0;
    } else if(tt(x | lava, lava, lava, lava, lava, lava, x | lava, x)) {
        id = 1;
    } else if(tt(x | lava, x, x | lava, lava, lava, lava, lava, lava)) {
        id = 2;
    } else if(tt(lava, lava, x | lava, x, x | lava, lava, lava, lava)) {
        id = 3;
    } else if(tt(x | lava, lava, lava, lava, x | lava, x, x | lava, x)) {
        id = 4;
    } else if(tt(x | lava, x, x | lava, lava, lava, lava, x | lava, x)) {
        id = 5;
    } else if(tt(x | lava, x, x | lava, x, x | lava, lava, lava, lava)) {
        id = 6;
    } else if(tt(lava, lava, x | lava, x, x | lava, x, x | lava, lava)) {
        id = 7;
    } else if(tt(x | lava, x, x | lava, lava, x | lava, x, x | lava, lava)) {
        id = 8;
    } else if(tt(x | lava, lava, x | lava, x, x | lava, lava, x | lava, x)) {
        id = 9;
    } else if(tt(x | lava, lava, x | lava, x, x | lava, x, x | lava, x)) {
        id = 10;
    } else if(tt(x | lava, x, x | lava, lava, x | lava, x, x | lava, x)) {
        id = 11;
    } else if(tt(x | lava, x, x | lava, x, x | lava, lava, x | lava, x)) {
        id = 12;
    } else if(tt(x | lava, x, x | lava, x, x | lava, x, x | lava, lava)) {
        id = 13;
    } else if(tt(lava, lava, x, lava, lava, lava, x, lava)) {
        id = 14;
    } else if(tt(x, lava, lava, lava, x, lava, lava, lava)) {
        id = 15;
    } else if(tt(lava, lava, lava, lava, lava, lava, x, lava)) {
        id = 16;
    } else if(tt(x, lava, lava, lava, lava, lava, lava, lava)) {
        id = 17;
    } else if(tt(lava, lava, x, lava, lava, lava, lava, lava)) {
        id = 18;
    } else if(tt(lava, lava, lava, lava, x, lava, lava, lava)) {
        id = 19;
    } else if(tt(x, lava, lava, lava, lava, lava, x, lava)) {
        id = 20;
    } else if(tt(x, lava, x, lava, lava, lava, lava, lava)) {
        id = 21;
    } else if(tt(lava, lava, x, lava, x, lava, lava, lava)) {
        id = 22;
    } else if(tt(lava, lava, lava, lava, x | lava, lava, x, lava)) {
        id = 23;
    } else if(tt(x, lava, x, lava, lava, lava, x, lava)) {
        id = 24;
    } else if(tt(x, lava, x, lava, x, lava, lava, lava)) {
        id = 25;
    } else if(tt(lava, lava, x, lava, x, lava, x, lava)) {
        id = 26;
    } else if(tt(x, lava, lava, lava, x, lava, x, lava)) {
        id = 27;
    } else if(tt(x, lava, x, lava, x, lava, x, lava)) {
        id = 28;
    } else if(tt(x | lava, lava, lava, lava, x | lava, x, x | lava, lava)) {
        id = 29;
    } else if(tt(lava, lava, x | lava, lava, x | lava, x, x | lava, lava)) {
        id = 30;
    } else if(tt(x | lava, lava, x | lava, lava, x | lava, x, x | lava, lava)) {
        id = 31;
    } else if(tt(x | lava, lava, x | lava, lava, lava, lava, x | lava, x)) {
        id = 32;
    } else if(tt(x | lava, lava, lava, lava, x | lava, lava, x | lava, x)) {
        id = 33;
    } else if(tt(x | lava, lava, x | lava, lava, x | lava, lava, x | lava, x)) {
        id = 34;
    } else if(tt(x | lava, x, x | lava, lava, x | lava, lava, lava, lava)) {
        id = 35;
    } else if(tt(x | lava, x, x | lava, lava, lava, lava, x | lava, lava)) {
        id = 36;
    } else if(tt(x | lava, x, x | lava, lava, x | lava, lava, x | lava, lava)) {
        id = 37;
    } else if(tt(lava, lava, x | lava, x, x | lava, lava, x | lava, lava)) {
        id = 38;
    } else if(tt(x | lava, lava, x | lava, x, x | lava, lava, lava, lava)) {
        id = 39;
    } else if(tt(x | lava, lava, x | lava, x, x | lava, lava, x | lava, lava)) {
        id = 40;
    } else if(tt(x | lava, lava, x | lava, lava, x | lava, x, x | lava, x)) {
        id = 41;
    } else if(tt(x | lava, x, x | lava, lava, x | lava, lava, x | lava, x)) {
        id = 42;
    } else if(tt(x | lava, x, x | lava, x, x | lava, lava, x | lava, lava)) {
        id = 43;
    } else if(tt(x | lava, lava, x | lava, x, x | lava, x, x | lava, lava)) {
        id = 44;
    } else if(tt(x | lava, x, x | lava, x, x | lava, x, x | lava, x)) {
        id = 45;
    }

    return id;
}
