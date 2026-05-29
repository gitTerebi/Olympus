#ifndef ROAD_TOOL_H
#define ROAD_TOOL_H

#include <vector>

class GameBoard;
class eTile;

// Builds the L-shape road preview/build path from a press tile to the hover
// tile. Tiles covered by a building are left in the list; the caller draws
// them red in preview and skips them when building.
//
// Stateful: remembers which axis the drag moved first so the fixed leg stays
// put and the L extends instead of flipping when the pointer crosses the
// diagonal. Call reset() on mouse press/release; noteDrag() on motion.
class eRoadTool {
public:
    void reset() { mFirstAxis = 0; }

    // Latch the first axis the drag moved along (once per drag). No-op until
    // hover leaves the press tile.
    void noteDrag(int pressedTX, int pressedTY, int hoverTX, int hoverTY);

    // Tile list press -> hover. Empty if start tile invalid.
    std::vector<eTile*> lShapeTiles(GameBoard* board,
                                    int pressedTX, int pressedTY,
                                    int hoverTX, int hoverTY) const;

    // Same list in hover -> press order, for the build/cost loop.
    std::vector<eTile*> tilesHoverToPress(GameBoard* board,
                                          int pressedTX, int pressedTY,
                                          int hoverTX, int hoverTY) const;

private:
    int mFirstAxis = 0; // 0 unset, 1 x-first, 2 y-first
};

#endif // ROAD_TOOL_H
