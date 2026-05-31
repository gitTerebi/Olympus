#ifndef FORMATION_FACING_H
#define FORMATION_FACING_H

namespace eFormationFacing {

// Snap a raw direction vector to one of the 8 axis-aligned/diagonal unit
// vectors. Iso tile drags hit diagonals very easily, so a near-even vector is
// required before snapping to a diagonal axis.
void snapBiased8Way(int rawDX, int rawDY, int& dx, int& dy);

// Map a snapped formation-line perpendicular (dx,dy) to a facing angle in
// degrees (0/45/.../315), matching SoldierBanner::soldierOrientation.
int snappedDragFacing(int dx, int dy);

// Given a direction vector (target - from), compute the formation facing and
// line axis so the formation faces along the vector. lineDX/lineDY is the
// formation width axis (perpendicular to facing); falls back to (1,0).
void facingAndLineToward(int dx, int dy,
                         int& facing, int& lineDX, int& lineDY);

// Tile-space unit vector the formation faces toward for a given facing angle,
// matching SoldierBanner::soldierOrientation (180 == +Y/bottom, 0 == -Y/top).
void facingFrontVector(int facing, int& fx, int& fy);

}

#endif // FORMATION_FACING_H
