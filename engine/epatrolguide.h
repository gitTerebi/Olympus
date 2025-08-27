#ifndef EPATROLGUIDE_H
#define EPATROLGUIDE_H

#include "eorientation.h"

struct ePatrolGuide {
    int fX;
    int fY;

    void moveInDirection(const eOrientation o) {
        switch(o) {
        case eOrientation::topRight:
            fY--;
            break;
        case eOrientation::right:
            fX++;
            fY--;
            break;
        case eOrientation::bottomRight:
            fX++;
            break;
        case eOrientation::bottom:
            fX++;
            fY++;
            break;
        case eOrientation::bottomLeft:
            fY++;
            break;
        case eOrientation::left:
            fX--;
            fY++;
            break;
        case eOrientation::topLeft:
            fX--;
            break;
        case eOrientation::top:
            fX--;
            fY--;
            break;
        }
    }
};

#endif // EPATROLGUIDE_H
