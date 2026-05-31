#ifndef EHEATMAPTASK_H
#define EHEATMAPTASK_H

#include "../etask.h"

#include <functional>

#include "eheatmap.h"

enum class eBuildingType;
class eTileBase;

class eHeatMapTask : public eTask {
public:
    using eHeatGetter = std::function<eHeat(eBuildingType)>;
    using eTileHeatGetter = std::function<eHeat(eTileBase* const)>;
    using eFunc = std::function<void(eHeatMap&)>;
    eHeatMapTask(const eCityId cid,
                 const SDL_Rect& bRect,
                 const eHeatGetter& heatGetter,
                 const eFunc& finish);

    static void sRun(eThreadBoard& board,
                     const eCityId cid,
                     const SDL_Rect& bRect,
                     const eHeatGetter& heatGetter,
                     eHeatMap& map);

    void run(eThreadBoard& board);
    void finish();
private:
    const eHeatGetter mHeatGetter;
    const eFunc mFinish;
    const SDL_Rect mBRect;
    eHeatMap mMap;
};

#endif // EHEATMAPTASK_H
