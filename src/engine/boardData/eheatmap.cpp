#include "eheatmap.h"

#include "tile-helper.h"

#include <algorithm>
#include <cassert>

void eHeatMap::add(const eHeatMap& other) {
    assert(mDX == other.mDX);
    assert(mDY == other.mDY);
    assert(mWidth == other.mWidth);
    assert(mHeight == other.mHeight);
    for(int x = 0; x < mWidth; x++) {
        auto& tRow = mMap[x];
        const auto& oRow = other.mMap[x];
        for(int y = 0; y < mHeight; y++) {
            auto& tCell = tRow[y];
            const auto& oCell = oRow[y];

            if(tCell.fEnabled || oCell.fEnabled) {
                tCell.fEnabled = true;
                tCell.fAppeal += oCell.fAppeal;
            } else {
                tCell.fEnabled = false;
                tCell.fAppeal = 0;
            }
        }
    }
}

void eHeatMap::multiply(const eHeatMap& other) {
    assert(mDX == other.mDX);
    assert(mDY == other.mDY);
    assert(mWidth == other.mWidth);
    assert(mHeight == other.mHeight);
    for(int x = 0; x < mWidth; x++) {
        auto& tRow = mMap[x];
        const auto& oRow = other.mMap[x];
        for(int y = 0; y < mHeight; y++) {
            auto& tCell = tRow[y];
            const auto& oCell = oRow[y];

            if(tCell.fEnabled && oCell.fEnabled) {
                tCell.fEnabled = true;
                tCell.fAppeal *= oCell.fAppeal;
            } else {
                tCell.fEnabled = false;
                tCell.fAppeal = 0;
            }
        }
    }
}

void eHeatMap::reset() {
    initialize(mDX, mDY, mWidth, mHeight);
}

void eHeatMap::initialize(const int dx, const int dy,
                          const int w, const int h) {
    mMap.clear();
    mDX = dx;
    mDY = dy;
    mWidth = w;
    mHeight = h;
    if(w > 0) mMap.reserve(w);
    for(int i = 0; i < w; i++) {
        auto& row = mMap.emplace_back();
        if(h > 0) row.reserve(h);
        for(int j = 0; j < h; j++) {
            row.emplace_back();
        }
    }
}

void eHeatMap::addHeat(const eHeat& a,
                       const SDL_Rect& tileRect) {
    addHeat(a, tileRect.x, tileRect.y,
               tileRect.w, tileRect.h);
}

void eHeatMap::addHeat(const eHeat& a,
                       const int ax, const int ay,
                       const int sw, const int sh) {
    const int r = a.fRange;
    if(r <= 0) return;
    const int step = a.fStepTiles > 0 ? a.fStepTiles : 1;
    const int delta = a.fStepSize;
    const int v0 = a.fValue;
    // Vanilla ring formula. Compute Chebyshev ring index from tile edge.
    for(int x = ax - r; x <= ax + sw - 1 + r; x++) {
        for(int y = ay - r; y <= ay + sh - 1 + r; y++) {
            // distance to nearest tile of footprint (Chebyshev, axis-aligned)
            int dx = 0;
            if(x < ax) dx = ax - x;
            else if(x >= ax + sw) dx = x - (ax + sw - 1);
            int dy = 0;
            if(y < ay) dy = ay - y;
            else if(y >= ay + sh) dy = y - (ay + sh - 1);
            const int dist = std::max(dx, dy);
            if(dist <= 0) continue;       // skip building footprint itself
            if(dist > r) continue;
            const int ring = (dist - 1) / step + 1;   // 1-based ring index
            const int appeal = v0 + (ring - 1) * delta;
            if(appeal == 0) continue;
            int dtx;
            int dty;
            TileHelper::tileIdToDTileId(x, y, dtx, dty);
            addHeat(dtx, dty, static_cast<double>(appeal));
        }
    }
}

void eHeatMap::addHeat(const int x, const int y,
                       const double a) {
    if(x < mDX || y < mDY ||
       x >= mWidth + mDX || y >= mHeight + mDY) return;
    auto& tile = mMap[x - mDX][y - mDY];
    tile.fEnabled = true;
    tile.fAppeal += a;
}

void eHeatMap::setOutsideRange(const int x, const int y) {
    if(x < mDX || y < mDY ||
       x >= mWidth + mDX || y >= mHeight + mDY) return;
    auto& tile = mMap[x - mDX][y - mDY];
    tile.fOutsideRange = true;
}

bool eHeatMap::enabled(const int x, const int y) const {
    const auto& t = mMap[x - mDX][y - mDY];
    return t.fEnabled && !t.fOutsideRange;
}

double eHeatMap::heat(const int x, const int y) const {
    return mMap[x - mDX][y - mDY].fAppeal;
}

void eHeatMap::set(const int x, const int y,
                   const bool e, const double h) {
    auto& tile = mMap[x - mDX][y - mDY];
    tile.fEnabled = e;
    tile.fAppeal = h;
}
