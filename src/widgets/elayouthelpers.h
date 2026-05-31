#ifndef ELAYOUTHELPERS_H
#define ELAYOUTHELPERS_H

#include <initializer_list>
#include <vector>

class eWidget;
class eMainWindow;

namespace eLayoutHelpers {

// ── flex helpers ──────────────────────────────────────────────────────────────

struct eFlexItem {
    eWidget* widget = nullptr;
    int      size   = 0;   // fixed size along main axis; 0 = use widget's natural size
    int      grow   = 0;   // flex-grow; leftover space distributed proportionally
};

enum class eJustify { start, center, end, spaceBetween, spaceAround };
enum class eAlign   { start, center, end, stretch };

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
// Col width = max(containerW, widest child). align stretch fills children to
// that width; wrap-labels rebreak their text to it.
eWidget* flexCol(eMainWindow* window,
                 int containerH,
                 std::initializer_list<eFlexItem> items,
                 eFlexParams params = {});

eWidget* flexCol(eMainWindow* window,
                 int containerH,
                 const std::vector<eFlexItem>& items,
                 eFlexParams params = {});

eWidget* flexCol(eMainWindow* window,
                 int containerW,
                 int containerH,
                 std::initializer_list<eFlexItem> items,
                 eFlexParams params = {});

eWidget* flexCol(eMainWindow* window,
                 int containerW,
                 int containerH,
                 const std::vector<eFlexItem>& items,
                 eFlexParams params = {});

} // namespace eLayoutHelpers

#endif // ELAYOUTHELPERS_H
