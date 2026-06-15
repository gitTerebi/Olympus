#ifndef ELAYOUTHELPERS_H
#define ELAYOUTHELPERS_H

#include <initializer_list>
#include <vector>

class eWidget;
class MainWindow;

namespace eLayoutHelpers {

// ── flex helpers ──────────────────────────────────────────────────────────────

struct eFlexItem {
    eWidget* widget = nullptr;
    int      size   = 0;   // fixed size along main axis; 0 = use widget's natural size
    int      grow   = 0;   // flex-grow; leftover space distributed proportionally
    int      shrink = 0;   // flex-shrink; overflow is removed proportionally
    int      minSize = 0;  // minimum main-axis size
    int      maxSize = 0;  // maximum main-axis size; 0 = no maximum
};

enum class eJustify { start, center, end, spaceBetween, spaceAround };
enum class eAlign   { start, center, end, stretch };
enum class eFlexDirection { row, column };

struct eFlexParams {
    int      gap     = 0;
    eJustify justify = eJustify::start;
    eAlign   align   = eAlign::start;
};

eWidget* createFlexContainer(MainWindow* window,
                             int containerW,
                             int containerH,
                             eFlexDirection direction,
                             std::initializer_list<eFlexItem> items,
                             eFlexParams params = {});

eWidget* createFlexContainer(MainWindow* window,
                             int containerW,
                             int containerH,
                             eFlexDirection direction,
                             const std::vector<eFlexItem>& items,
                             eFlexParams params = {});

void updateFlexContainerLayout(eWidget* container,
                               eFlexDirection direction,
                               std::initializer_list<eFlexItem> items,
                               eFlexParams params = {});

} // namespace eLayoutHelpers

#endif // ELAYOUTHELPERS_H
