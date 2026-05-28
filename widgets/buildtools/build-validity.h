#ifndef BUILD_VALIDITY_H
#define BUILD_VALIDITY_H

#include "engine/eorientation.h"

class eGameBoard;
class eTile;
enum class eResourceType;
enum class eCityId;
enum class ePlayerId;

namespace BuildValidity {

// Single agora space tile accepts a vendor for resType.
bool canBuildVendor(eGameBoard* board, int tx, int ty,
                    eResourceType resType);

// Water tile reachable from this city's river entry by water-only path.
bool waterTileHasAccessToSea(eGameBoard* board, eCityId viewedCity,
                             int tx, int ty);

// 2x2 fishery footprint anchored at (tx,ty-1)..(tx+1,ty); picks orientation.
bool canBuildFishery(eGameBoard* board, int tx, int ty,
                     eDiagonalOrientation& o);

// 3x3 trireme wharf footprint; picks orientation.
bool canBuildTriremeWharf(eGameBoard* board, int tx, int ty,
                          eDiagonalOrientation& o);

// Pier = fishery + 5x5 land plot per orientation.
bool canBuildPier(eGameBoard* board, int tx, int ty,
                  eDiagonalOrientation& o, eCityId cid,
                  ePlayerId pid, bool forestAllowed);

// Thin wrapper for board's avenue rule. Lives here for symmetry.
bool canBuildAvenue(eGameBoard* board, eTile* t, eCityId cid,
                    ePlayerId pid, bool forestAllowed);

} // namespace BuildValidity

#endif // BUILD_VALIDITY_H
