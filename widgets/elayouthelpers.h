#ifndef ELAYOUTHELPERS_H
#define ELAYOUTHELPERS_H

#include <initializer_list>
#include <utility>
#include <vector>

class eWidget;
class eMainWindow;

namespace eLayoutHelpers {

// ── original helpers ─────────────────────────────────────────────────────────

// Fixed-column row. Each pair: {widget, column width}. Returns sized row.
eWidget* hRow(eMainWindow* window,
              std::initializer_list<std::pair<eWidget*, int>> cols,
              int gap);

// Stack children of parent vertically with gap, then fitContent.
void vStack(eWidget* parent, int gap);

// ── flex helpers ──────────────────────────────────────────────────────────────

struct eFlexItem {
    eWidget* widget = nullptr;
    int      size   = 0;   // fixed size along main axis; 0 = use widget's natural size
    int      grow   = 0;   // flex-grow; leftover space distributed proportionally
};

enum class eJustify { start, center, end, spaceBetween, spaceAround };
enum class eAlign   { start, center, end };

struct eFlexParams {
    int      gap     = 0;
    eJustify justify = eJustify::start;
    eAlign   align   = eAlign::start;
};

// Flex row. containerW is the total available width.
// Returns a row widget of size containerW × (tallest child).
eWidget* flexRow(eMainWindow* window,
                 int containerW,
                 std::initializer_list<eFlexItem> items,
                 eFlexParams params = {});

// Flex column. containerH = 0 means fit-content height.
// Returns a column widget of size (widest child) × height.
eWidget* flexCol(eMainWindow* window,
                 int containerH,
                 std::initializer_list<eFlexItem> items,
                 eFlexParams params = {});

eWidget* flexCol(eMainWindow* window,
                 int containerH,
                 const std::vector<eFlexItem>& items,
                 eFlexParams params = {});

} // namespace eLayoutHelpers

#endif // ELAYOUTHELPERS_H
