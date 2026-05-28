#ifndef COLUMN_TOOL_H
#define COLUMN_TOOL_H

#include <vector>

#include "engine/eorientation.h"

class eGameBoard;

namespace ColumnTool {

// A* path along buildable tiles that already host columns or are empty.
// From hover to press. Returns false if no path within search budget.
bool path(eGameBoard* board,
          int pressedTX, int pressedTY,
          int hoverTX, int hoverTY,
          std::vector<eOrientation>& out);

} // namespace ColumnTool

#endif // COLUMN_TOOL_H
