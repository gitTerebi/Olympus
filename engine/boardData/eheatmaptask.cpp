#include "eheatmaptask.h"

#include "../thread/ethreadboard.h"

eHeatMapTask::eHeatMapTask(const eCityId cid,
                           const SDL_Rect& bRect,
                           const eHeatGetter& heatGetter,
                           const eFunc& finish) :
    eTask(cid), mHeatGetter(heatGetter), mFinish(finish),
    mBRect(bRect) {}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const SDL_Rect bRect,
                        const eHeatGetter& heatGetter,
                        eHeatMap& map) {
    map.initialize(bRect.x, bRect.y, bRect.w, bRect.h);
    for(int tx = bRect.x; tx < bRect.x + bRect.w; tx++) {
        for(int ty = bRect.y; ty < bRect.y + bRect.h; ty++) {
            const auto t = board.dtile(tx, ty);
            const auto& ub = t->underBuilding();
            const auto ubt = ub.type();
            if(ubt == eBuildingType::none) continue;
            const auto rect = ub.tileRect();
            const int ttx = t->x();
            const int tty = t->y();
            if(ttx != rect.x || tty != rect.y) continue;
            const auto a = heatGetter(ubt);
            map.addHeat(a, rect);
        }
    }
}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const SDL_Rect bRect,
                        const eTileHeatGetter& heatGetter,
                        eHeatMap& map) {
    map.initialize(bRect.x, bRect.y, bRect.w, bRect.h);
    for(int tx = bRect.x; tx < bRect.x + bRect.w; tx++) {
        for(int ty = bRect.y; ty < bRect.y + bRect.h; ty++) {
            const auto t = board.dtile(tx, ty);
            const auto a = heatGetter(t);
            map.addHeat(a, {t->x(), t->y(), 1, 1});
        }
    }
}

void eHeatMapTask::run(eThreadBoard& board) {
    sRun(board, mBRect, mHeatGetter, mMap);
}

void eHeatMapTask::finish() {
    if(mFinish) mFinish(mMap);
}
