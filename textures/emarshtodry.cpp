#include "emarshtodry.h"

#include "esurroundingterrain.h"

int eMarshToDry::get(eTile* const tile, const eWorldDirection dir) {
    const auto x = eTerrain::all ^ eTerrain::marsh;
    const auto marsh = eTerrain::marsh;

    if(tile->terrain() != marsh) return -1;

    eSurroundingTerrain tt(tile, dir);

    int id = -1;

    if(tt(marsh, marsh, marsh, marsh, x | marsh, x, x | marsh, marsh)) {
        id = 0;
    } else if(tt(x | marsh, marsh, marsh, marsh, marsh, marsh, x | marsh, x)) {
        id = 1;
    } else if(tt(x | marsh, x, x | marsh, marsh, marsh, marsh, marsh, marsh)) {
        id = 2;
    } else if(tt(marsh, marsh, x | marsh, x, x | marsh, marsh, marsh, marsh)) {
        id = 3;
    } else if(tt(x | marsh, marsh, marsh, marsh, x | marsh, x, x | marsh, x)) {
        id = 4;
    } else if(tt(x | marsh, x, x | marsh, marsh, marsh, marsh, x | marsh, x)) {
        id = 5;
    } else if(tt(x | marsh, x, x | marsh, x, x | marsh, marsh, marsh, marsh)) {
        id = 6;
    } else if(tt(marsh, marsh, x | marsh, x, x | marsh, x, x | marsh, marsh)) {
        id = 7;
    } else if(tt(x | marsh, x, x | marsh, marsh, x | marsh, x, x | marsh, marsh)) {
        id = 8;
    } else if(tt(x | marsh, marsh, x | marsh, x, x | marsh, marsh, x | marsh, x)) {
        id = 9;
    } else if(tt(x | marsh, marsh, x | marsh, x, x | marsh, x, x | marsh, x)) {
        id = 10;
    } else if(tt(x | marsh, x, x | marsh, marsh, x | marsh, x, x | marsh, x)) {
        id = 11;
    } else if(tt(x | marsh, x, x | marsh, x, x | marsh, marsh, x | marsh, x)) {
        id = 12;
    } else if(tt(x | marsh, x, x | marsh, x, x | marsh, x, x | marsh, marsh)) {
        id = 13;
    } else if(tt(marsh, marsh, x, marsh, marsh, marsh, x, marsh)) {
        id = 14;
    } else if(tt(x, marsh, marsh, marsh, x, marsh, marsh, marsh)) {
        id = 15;
    } else if(tt(marsh, marsh, marsh, marsh, marsh, marsh, x, marsh)) {
        id = 16;
    } else if(tt(x, marsh, marsh, marsh, marsh, marsh, marsh, marsh)) {
        id = 17;
    } else if(tt(marsh, marsh, x, marsh, marsh, marsh, marsh, marsh)) {
        id = 18;
    } else if(tt(marsh, marsh, marsh, marsh, x, marsh, marsh, marsh)) {
        id = 19;
    } else if(tt(x, marsh, marsh, marsh, marsh, marsh, x, marsh)) {
        id = 20;
    } else if(tt(x, marsh, x, marsh, marsh, marsh, marsh, marsh)) {
        id = 21;
    } else if(tt(marsh, marsh, x, marsh, x, marsh, marsh, marsh)) {
        id = 22;
    } else if(tt(marsh, marsh, marsh, marsh, x | marsh, marsh, x, marsh)) {
        id = 23;
    } else if(tt(x, marsh, x, marsh, marsh, marsh, x, marsh)) {
        id = 24;
    } else if(tt(x, marsh, x, marsh, x, marsh, marsh, marsh)) {
        id = 25;
    } else if(tt(marsh, marsh, x, marsh, x, marsh, x, marsh)) {
        id = 26;
    } else if(tt(x, marsh, marsh, marsh, x, marsh, x, marsh)) {
        id = 27;
    } else if(tt(x, marsh, x, marsh, x, marsh, x, marsh)) {
        id = 28;
    } else if(tt(x | marsh, marsh, marsh, marsh, x | marsh, x, x | marsh, marsh)) {
        id = 29;
    } else if(tt(marsh, marsh, x | marsh, marsh, x | marsh, x, x | marsh, marsh)) {
        id = 30;
    } else if(tt(x | marsh, marsh, x | marsh, marsh, x | marsh, x, x | marsh, marsh)) {
        id = 31;
    } else if(tt(x | marsh, marsh, x | marsh, marsh, marsh, marsh, x | marsh, x)) {
        id = 32;
    } else if(tt(x | marsh, marsh, marsh, marsh, x | marsh, marsh, x | marsh, x)) {
        id = 33;
    } else if(tt(x | marsh, marsh, x | marsh, marsh, x | marsh, marsh, x | marsh, x)) {
        id = 34;
    } else if(tt(x | marsh, x, x | marsh, marsh, x | marsh, marsh, marsh, marsh)) {
        id = 35;
    } else if(tt(x | marsh, x, x | marsh, marsh, marsh, marsh, x | marsh, marsh)) {
        id = 36;
    } else if(tt(x | marsh, x, x | marsh, marsh, x | marsh, marsh, x | marsh, marsh)) {
        id = 37;
    } else if(tt(marsh, marsh, x | marsh, x, x | marsh, marsh, x | marsh, marsh)) {
        id = 38;
    } else if(tt(x | marsh, marsh, x | marsh, x, x | marsh, marsh, marsh, marsh)) {
        id = 39;
    } else if(tt(x | marsh, marsh, x | marsh, x, x | marsh, marsh, x | marsh, marsh)) {
        id = 40;
    } else if(tt(x | marsh, marsh, x | marsh, marsh, x | marsh, x, x | marsh, x)) {
        id = 41;
    } else if(tt(x | marsh, x, x | marsh, marsh, x | marsh, marsh, x | marsh, x)) {
        id = 42;
    } else if(tt(x | marsh, x, x | marsh, x, x | marsh, marsh, x | marsh, marsh)) {
        id = 43;
    } else if(tt(x | marsh, marsh, x | marsh, x, x | marsh, x, x | marsh, marsh)) {
        id = 44;
    } else if(tt(x | marsh, x, x | marsh, x, x | marsh, x, x | marsh, x)) {
        id = 45;
    }

    return id;
}
