#ifndef ROAD_TOOL_H
#define ROAD_TOOL_H

#include <vector>

class GameBoard;
class eTile;

namespace RoadTool {

// Build L-shape tile list from press to hover in tile coords. Walks dominant
// of |dx|,|dy| to hover's coord on that axis, then perpendicular axis to hover.
// Returned tiles are ordered press -> hover. Empty if start tile invalid.
std::vector<eTile*> lShapeTiles(GameBoard* board,
                                int pressedTX, int pressedTY,
                                int hoverTX, int hoverTY);

// Tile list in hover -> press order, suitable for the build/cost loop.
std::vector<eTile*> tilesHoverToPress(GameBoard* board,
                                      int pressedTX, int pressedTY,
                                      int hoverTX, int hoverTY);

} // namespace RoadTool

#endif // ROAD_TOOL_H
