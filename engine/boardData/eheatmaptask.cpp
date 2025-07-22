#include "eheatmaptask.h"

#include "../thread/ethreadboard.h"

eHeatMapTask::eHeatMapTask(const eCityId cid,
                           const SDL_Rect& bRect,
                           const eHeatGetter& heatGetter,
                           const eFunc& finish) :
    eTask(cid), mHeatGetter(heatGetter), mFinish(finish),
    mBRect(bRect) {
    setRelevance(eStateRelevance::buildings);
}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const eCityId cid,
                        const SDL_Rect bRect,
                        const eHeatGetter& heatGetter,
                        eHeatMap& map) {
    map.initialize(bRect.x, bRect.y, bRect.w, bRect.h);
    for(int tx = bRect.x; tx < bRect.x + bRect.w; tx++) {
        for(int ty = bRect.y; ty < bRect.y + bRect.h; ty++) {
            const auto t = board.dtile(tx, ty);
            if(t->cityId() != cid) map.setOutsideRange(tx, ty);
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

void eHeatMapTask::run(eThreadBoard& board) {
    sRun(board, cid(), mBRect, mHeatGetter, mMap);
}

void eHeatMapTask::finish() {
    if(mFinish) mFinish(mMap);
}
