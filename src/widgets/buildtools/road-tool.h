#ifndef ROAD_TOOL_H
#define ROAD_TOOL_H

#include <vector>

class GameBoard;
class eTile;

namespace LShapeTool {

// Tile list press -> hover along L-path. firstAxis: 0=unlatched, 1=x-first, 2=y-first.
std::vector<eTile*> tiles(GameBoard* board,
                          int pressedTX, int pressedTY,
                          int hoverTX, int hoverTY,
                          int firstAxis);

} // namespace LShapeTool

// Stateful wrapper: remembers which axis the drag moved first so the fixed leg
// stays put and the L extends instead of flipping. Call reset() on press;
// noteDrag() on motion.
class eRoadTool {
public:
    void reset() { mFirstAxis = 0; }

    void noteDrag(int pressedTX, int pressedTY, int hoverTX, int hoverTY);

    // Tile list press -> hover. Empty if start tile invalid.
    std::vector<eTile*> lShapeTiles(GameBoard* board,
                                    int pressedTX, int pressedTY,
                                    int hoverTX, int hoverTY) const;

    // Same list in hover -> press order, for the build/cost loop.
    std::vector<eTile*> tilesHoverToPress(GameBoard* board,
                                          int pressedTX, int pressedTY,
                                          int hoverTX, int hoverTY) const;

    int firstAxis() const { return mFirstAxis; }

private:
    int mFirstAxis = 0; // 0 unset, 1 x-first, 2 y-first
};

#endif // ROAD_TOOL_H
