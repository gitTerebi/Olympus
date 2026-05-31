#ifndef BRIDGE_TOOL_H
#define BRIDGE_TOOL_H

#include <vector>

#include "engine/eterrain.h"

class eTile;

namespace BridgeTool {

// Collects the tile run that would form a bridge starting at t across terr.
// Sets rotated when the bridge runs along the topLeft/bottomRight diagonal
// instead of topRight/bottomLeft. Returns false if no valid run found.
bool tiles(eTile* t, eTerrain terr,
           std::vector<eTile*>& out, bool& rotated);

} // namespace BridgeTool

#endif // BRIDGE_TOOL_H
