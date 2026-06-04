#include "elayouthelpers.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include "elabel.h"
#include "ewidget.h"
#include "emainwindow.h"

namespace eLayoutHelpers {

// ── flex ─────────────────────────────────────────────────────────────────────

static void layoutFlexItems(eWidget* container,
                            eFlexDirection direction,
                            const std::vector<eFlexItem>& items,
                            eFlexParams params);

eWidget* createFlexContainer(eMainWindow* const window,
                             const int containerW,
                             const int containerH,
                             const eFlexDirection direction,
                             std::initializer_list<eFlexItem> items,
                             const eFlexParams params) {
    return createFlexContainer(window, containerW, containerH, direction,
                               std::vector<eFlexItem>(items), params);
}

eWidget* createFlexContainer(eMainWindow* const window,
                             const int containerW,
                             const int containerH,
                             const eFlexDirection direction,
                             const std::vector<eFlexItem>& items,
                             const eFlexParams params) {
    std::vector<eFlexItem> layoutItems(items);
    if(direction == eFlexDirection::column &&
       params.align == eAlign::stretch &&
       containerW > 0) {
        for(auto& it : layoutItems) {
            it.widget->setWidth(containerW);
            const auto label = dynamic_cast<eLabel*>(it.widget);
            if(label) {
                label->setWrapWidth(containerW);
                label->fitContent();
            }
        }
    }

    const auto container = new eWidget(window);
    container->setNoPadding();
    container->resize(containerW, containerH);
    for(const auto& it : layoutItems) {
        container->addWidget(it.widget);
    }
    layoutFlexItems(container, direction, layoutItems, params);
    return container;
}

static void layoutFlexItems(eWidget* const container,
                            const eFlexDirection direction,
                            const std::vector<eFlexItem>& items,
                            const eFlexParams params) {
    std::vector<eFlexItem> its(items);
    const int n = (int)its.size();
    if(n == 0) return;

    const bool isRow = direction == eFlexDirection::row;
    const auto mainSize = [isRow](const eWidget* const w) {
        return isRow ? w->width() : w->height();
    };
    const auto crossSize = [isRow](const eWidget* const w) {
        return isRow ? w->height() : w->width();
    };
    const auto setMainSize = [isRow](eWidget* const w, const int s) {
        if(isRow) w->setWidth(s);
        else w->setHeight(s);
    };
    const auto setCrossSize = [isRow](eWidget* const w, const int s) {
        if(isRow) w->setHeight(s);
        else w->setWidth(s);
    };
    const auto moveItem = [isRow](eWidget* const w, const int main, const int cross) {
        if(isRow) w->move(main, cross);
        else w->move(cross, main);
    };

    for(auto& it : its) {
        if(it.size == 0) it.size = mainSize(it.widget);
        it.size = std::max(it.size, it.minSize);
        if(it.maxSize > 0) it.size = std::min(it.size, it.maxSize);
    }

    const int gap = params.gap;
    const int totalGaps = n > 1 ? gap * (n - 1) : 0;
    int fixedTotal = std::accumulate(its.begin(), its.end(), 0,
                         [](int s, const eFlexItem& i){ return s + i.size; });
    int containerMain = isRow ? container->width() : container->height();
    if(containerMain == 0) {
        containerMain = fixedTotal + totalGaps;
        setMainSize(container, containerMain);
    }
    int leftover = containerMain - fixedTotal - totalGaps;
    const int totalGrow = std::accumulate(its.begin(), its.end(), 0,
                              [](int s, const eFlexItem& i){ return s + i.grow; });
    if(totalGrow > 0 && leftover > 0) {
        for(auto& it : its) {
            if(it.grow > 0) it.size += leftover * it.grow / totalGrow;
            if(it.maxSize > 0) it.size = std::min(it.size, it.maxSize);
        }
    } else if(leftover < 0) {
        const int totalShrink = std::accumulate(its.begin(), its.end(), 0,
            [](int s, const eFlexItem& i){ return s + i.shrink * i.size; });
        if(totalShrink > 0) {
            const int overflow = -leftover;
            for(auto& it : its) {
                if(it.shrink <= 0) continue;
                const int remove = overflow * it.shrink * it.size / totalShrink;
                it.size = std::max(it.minSize, it.size - remove);
            }
        }
    }
    int maxCross = isRow ? container->height() : container->width();
    for(const auto& it : its) maxCross = std::max(maxCross, crossSize(it.widget));
    setCrossSize(container, maxCross);

    int start = 0;
    int extraGap = 0;
    const int usedMain = std::accumulate(its.begin(), its.end(), 0,
                          [](int s, const eFlexItem& i){ return s + i.size; })
                      + totalGaps;
    const int free = containerMain - usedMain;
    switch(params.justify) {
    case eJustify::center: start = free / 2; break;
    case eJustify::end: start = free; break;
    case eJustify::spaceBetween: extraGap = n > 1 ? free / (n - 1) : 0; break;
    case eJustify::spaceAround: start = free / (2 * n); extraGap = n > 1 ? free / n : 0; break;
    default: break;
    }

    int main = start;
    for(const auto& it : its) {
        int cross = 0;
        switch(params.align) {
        case eAlign::center: cross = (maxCross - crossSize(it.widget)) / 2; break;
        case eAlign::end: cross = maxCross - crossSize(it.widget); break;
        case eAlign::stretch: setCrossSize(it.widget, maxCross); break;
        default: break;
        }
        moveItem(it.widget, main, cross);
        setMainSize(it.widget, it.size);
        main += it.size + gap + extraGap;
    }
}

void updateFlexContainerLayout(eWidget* const container,
                               const eFlexDirection direction,
                               std::initializer_list<eFlexItem> items,
                               const eFlexParams params) {
    layoutFlexItems(container, direction, std::vector<eFlexItem>(items), params);
}

} // namespace eLayoutHelpers
