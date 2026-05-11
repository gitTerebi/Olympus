#include "elayouthelpers.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include "ewidget.h"
#include "emainwindow.h"

namespace eLayoutHelpers {

eWidget* hRow(eMainWindow* const window,
              std::initializer_list<std::pair<eWidget*, int>> cols,
              const int gap) {
    const auto row = new eWidget(window);
    row->setNoPadding();
    int x = 0;
    for(const auto& [w, colW] : cols) {
        row->addWidget(w);
        w->setX(x);
        w->setWidth(colW);
        x += colW + gap;
    }
    row->fitContent();
    return row;
}

void vStack(eWidget* const parent, const int gap) {
    parent->stackVertically(gap);
    parent->fitContent();
}

// ── flex ─────────────────────────────────────────────────────────────────────

eWidget* flexRow(eMainWindow* const window,
                 const int containerW,
                 std::initializer_list<eFlexItem> items,
                 const eFlexParams params) {
    const int gap     = params.gap;
    const eJustify justify = params.justify;
    const eAlign   align   = params.align;
    std::vector<eFlexItem> its(items);
    const int n = (int)its.size();

    // resolve natural sizes
    for(auto& it : its)
        if(it.size == 0) it.size = it.widget->width();

    // distribute grow
    const int totalGaps  = n > 1 ? gap * (n - 1) : 0;
    const int fixedTotal = std::accumulate(its.begin(), its.end(), 0,
                               [](int s, const eFlexItem& i){ return s + i.size; });
    const int leftover   = containerW - fixedTotal - totalGaps;
    const int totalGrow  = std::accumulate(its.begin(), its.end(), 0,
                               [](int s, const eFlexItem& i){ return s + i.grow; });
    if(totalGrow > 0 && leftover > 0) {
        for(auto& it : its)
            if(it.grow > 0)
                it.size += leftover * it.grow / totalGrow;
    }

    // max cross-axis size
    int maxH = 0;
    for(const auto& it : its) maxH = std::max(maxH, it.widget->height());

    // justify: compute start x and extra per-gap
    int startX  = 0;
    int extraGap = 0;
    const int usedW = std::accumulate(its.begin(), its.end(), 0,
                          [](int s, const eFlexItem& i){ return s + i.size; })
                      + totalGaps;
    const int free = containerW - usedW;
    switch(justify) {
    case eJustify::center:      startX = free / 2; break;
    case eJustify::end:         startX = free;     break;
    case eJustify::spaceBetween: extraGap = n > 1 ? free / (n - 1) : 0; break;
    case eJustify::spaceAround:  startX = free / (2 * n); extraGap = n > 1 ? free / n : 0; break;
    default: break;
    }

    const auto row = new eWidget(window);
    row->setNoPadding();
    row->resize(containerW, maxH);

    int x = startX;
    for(const auto& it : its) {
        row->addWidget(it.widget);
        int y = 0;
        switch(align) {
        case eAlign::center: y = (maxH - it.widget->height()) / 2; break;
        case eAlign::end:    y = maxH - it.widget->height();       break;
        default: break;
        }
        it.widget->move(x, y);
        it.widget->setWidth(it.size);
        x += it.size + gap + extraGap;
    }
    return row;
}

eWidget* flexCol(eMainWindow* const window,
                 const int containerH,
                 const std::vector<eFlexItem>& items,
                 const eFlexParams params) {
    const int gap     = params.gap;
    const eJustify justify = params.justify;
    const eAlign   align   = params.align;
    std::vector<eFlexItem> its(items);
    const int n = (int)its.size();

    for(auto& it : its)
        if(it.size == 0) it.size = it.widget->height();

    const int totalGaps  = n > 1 ? gap * (n - 1) : 0;
    const int fixedTotal = std::accumulate(its.begin(), its.end(), 0,
                               [](int s, const eFlexItem& i){ return s + i.size; });
    const int usedH      = fixedTotal + totalGaps;
    const int resolvedH  = containerH > 0 ? containerH : usedH;

    const int leftover  = resolvedH - usedH;
    const int totalGrow = std::accumulate(its.begin(), its.end(), 0,
                              [](int s, const eFlexItem& i){ return s + i.grow; });
    if(totalGrow > 0 && leftover > 0) {
        for(auto& it : its)
            if(it.grow > 0)
                it.size += leftover * it.grow / totalGrow;
    }

    int maxW = 0;
    for(const auto& it : its) maxW = std::max(maxW, it.widget->width());

    int startY   = 0;
    int extraGap = 0;
    const int usedFinal = std::accumulate(its.begin(), its.end(), 0,
                              [](int s, const eFlexItem& i){ return s + i.size; })
                          + totalGaps;
    const int free = resolvedH - usedFinal;
    switch(justify) {
    case eJustify::center:       startY = free / 2; break;
    case eJustify::end:          startY = free;     break;
    case eJustify::spaceBetween: extraGap = n > 1 ? free / (n - 1) : 0; break;
    case eJustify::spaceAround:  startY = free / (2 * n); extraGap = n > 1 ? free / n : 0; break;
    default: break;
    }

    const auto col = new eWidget(window);
    col->setNoPadding();
    col->resize(maxW, resolvedH);

    int y = startY;
    for(const auto& it : its) {
        col->addWidget(it.widget);
        int x = 0;
        switch(align) {
        case eAlign::center: x = (maxW - it.widget->width()) / 2; break;
        case eAlign::end:    x = maxW - it.widget->width();       break;
        default: break;
        }
        it.widget->move(x, y);
        it.widget->setHeight(it.size);
        y += it.size + gap + extraGap;
    }
    return col;
}

eWidget* flexCol(eMainWindow* const window,
                 const int containerH,
                 std::initializer_list<eFlexItem> items,
                 const eFlexParams params) {
    return flexCol(window, containerH, std::vector<eFlexItem>(items), params);
}

} // namespace eLayoutHelpers
