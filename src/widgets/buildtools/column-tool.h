#ifndef COLUMN_TOOL_H
#define COLUMN_TOOL_H

#include <vector>

#include "engine/eorientation.h"

class GameBoard;

namespace ColumnTool {

// L-shape path (same algo as road) from hover to press.
// firstAxis: 0=unlatched, 1=x-first, 2=y-first (from eRoadTool::firstAxis()).
// Returns false if start tile is invalid.
bool path(GameBoard* board,
          int pressedTX, int pressedTY,
          int hoverTX, int hoverTY,
          int firstAxis,
          std::vector<eOrientation>& out);

} // namespace ColumnTool

#endif // COLUMN_TOOL_H
