#ifndef EPYRAMID_H
#define EPYRAMID_H

#include "emonument.h"

enum class ePyramidType {
    modestPyramid, // 3x3
    pyramid, // 5x5
    greatPyramid, // 7x7
    majesticPyramid, // 9x9

    smallMonumentToTheSky, // 5x5
    monumentToTheSky, // 6x6
    grandMonumentToTheSky, // 8x8

    minorShrine, // 3x3
    shrine, // 6x6
    majorShrine, // 8x8

    pyramidToThePantheon, // 11x9
    altarOfOlympus, // 8x8
    templeOfOlympus, // 8x8
    observatoryKosmika, // 9x9
    museumAtlantika // 8x8
};

class ePyramid : public eMonument {
public:
    using eMonument::eMonument;
};

#endif // EPYRAMID_H
