#include "eheatmaptask.h"

#include "../thread/ethreadboard.h"

eHeatMapTask::eHeatMapTask(const eCityId cid,
                           const eHeatGetter& heatGetter,
                           const eFunc& finish) :
    eTask(cid), mHeatGetter(heatGetter), mFinish(finish) {

}

void eHeatMapTask::sRun(eThreadBoard& board,
                        const eHeatGetter& heatGetter,
                        eHeatMap& map) {
    const int w = board.width();
    const int h = board.height();
    map.initialize(w, h);
    for(int tx = 0; tx < w; tx++) {
        for(int ty = 0; ty < h; ty++) {
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
                        const eTileHeatGetter& heatGetter,
                        eHeatMap& map) {
    const int w = board.width();
    const int h = board.height();
    map.initialize(w, h);
    for(int tx = 0; tx < w; tx++) {
        for(int ty = 0; ty < h; ty++) {
            const auto t = board.dtile(tx, ty);
            const auto a = heatGetter(t);
            map.addHeat(a, {t->x(), t->y(), 1, 1});
        }
    }
}

void eHeatMapTask::run(eThreadBoard& board) {
    sRun(board, mHeatGetter, mMap);
}

void eHeatMapTask::finish() {
    if(mFinish) mFinish(mMap);
}
