#ifndef BUILD_VALIDITY_H
#define BUILD_VALIDITY_H

#include "engine/eorientation.h"

class GameBoard;
class eTile;
enum class eResourceType;
enum class eCityId;
enum class ePlayerId;

namespace BuildValidity {

// Single agora space tile accepts a vendor for resType.
bool canBuildVendor(GameBoard* board, int tx, int ty,
                    eResourceType resType);

// Water tile reachable from this city's river entry by water-only path.
bool waterTileHasAccessToSea(GameBoard* board, eCityId viewedCity,
                             int tx, int ty);

// 2x2 fishery footprint anchored at (tx,ty-1)..(tx+1,ty); picks orientation.
bool canBuildFishery(GameBoard* board, int tx, int ty,
                     eDiagonalOrientation& o);

// 3x3 trireme wharf footprint; picks orientation.
bool canBuildTriremeWharf(GameBoard* board, int tx, int ty,
                          eDiagonalOrientation& o);

// Pier = fishery + 5x5 land plot per orientation.
bool canBuildPier(GameBoard* board, int tx, int ty,
                  eDiagonalOrientation& o, eCityId cid,
                  ePlayerId pid, bool forestAllowed);

// Thin wrapper for board's avenue rule. Lives here for symmetry.
bool canBuildAvenue(GameBoard* board, eTile* t, eCityId cid,
                    ePlayerId pid, bool forestAllowed);

} // namespace BuildValidity

#endif // BUILD_VALIDITY_H
