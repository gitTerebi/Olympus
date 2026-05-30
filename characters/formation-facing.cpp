#include "formation-facing.h"

#include <cstdlib>

namespace eFormationFacing {

void snapBiased8Way(const int rawDX, const int rawDY,
                    int& dx, int& dy) {
    const int ax = std::abs(rawDX);
    const int ay = std::abs(rawDY);
    dx = rawDX == 0 ? 0 : (rawDX > 0 ? 1 : -1);
    dy = rawDY == 0 ? 0 : (rawDY > 0 ? 1 : -1);

    if(ax == 0 || ay == 0) return;

    const bool xDominant = ax > ay;
    const int major = xDominant ? ax : ay;
    const int minor = xDominant ? ay : ax;

    // Iso tile drags hit diagonals very easily; require a near-even drag
    // before snapping to a diagonal formation axis.
    if(minor*3 < major*2) {
        if(xDominant) {
            dy = 0;
        } else {
            dx = 0;
        }
    }
}

int snappedDragFacing(const int dx, const int dy) {
    if(dx < 0 && dy < 0) return 0;
    if(dx == 0 && dy < 0) return 45;
    if(dx > 0 && dy < 0) return 90;
    if(dx > 0 && dy == 0) return 135;
    if(dx > 0 && dy > 0) return 180;
    if(dx == 0 && dy > 0) return 225;
    if(dx < 0 && dy > 0) return 270;
    if(dx < 0 && dy == 0) return 315;
    return 0;
}

void facingAndLineToward(const int dx, const int dy,
                         int& facing, int& lineDX, int& lineDY) {
    // The formation line is perpendicular to the movement vector.
    snapBiased8Way(dy, -dx, lineDX, lineDY);
    if(lineDX == 0 && lineDY == 0) {
        lineDX = 1;
        lineDY = 0;
    }
    facing = snappedDragFacing(-lineDY, lineDX);
}

void facingFrontVector(const int facing, int& fx, int& fy) {
    const int f = ((facing % 360) + 360) % 360;
    switch(f) {
    case   0: fx =  0; fy = -1; break; // top
    case  45: fx =  1; fy = -1; break; // topRight
    case  90: fx =  1; fy =  0; break; // right
    case 135: fx =  1; fy =  1; break; // bottomRight
    case 180: fx =  0; fy =  1; break; // bottom
    case 225: fx = -1; fy =  1; break; // bottomLeft
    case 270: fx = -1; fy =  0; break; // left
    case 315: fx = -1; fy = -1; break; // topLeft
    default:  fx =  0; fy =  1; break;
    }
}

}
