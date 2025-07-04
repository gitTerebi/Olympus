#include "eaicityplan.h"

#include "engine/egameboard.h"

#include "evectorhelpers.h"

#include "engine/epathfinder.h"

#include "eaiconnectwithroadtask.h"
#include "etilehelper.h"

eAICityPlan::eAICityPlan(const eCityId cid) :
    mCid(cid) {}

eAIBoard eAICityPlan::aiBoard(const int w, const int h) const {
    eAIBoard result;
    result.initialize(w, h);
    for(const auto& d : mDistricts) {
        for(const auto& b : d.fBuildings) {
            const int xMin = b.fRect.x;
            const int xMax = xMin + b.fRect.w - 1;
            const int yMin = b.fRect.y;
            const int yMax = yMin + b.fRect.h - 1;
            for(int x = xMin; x <= xMax; x++) {
                for(int y = yMin; y <= yMax; y++) {
                    int dx;
                    int dy;
                    eTileHelper::tileIdToDTileId(x, y, dx, dy);
                    const auto t = result.tile(dx, dy);
                    t->fBuilding = b.fType;
                }
            }
        }
    }
    return result;
}

int eAICityPlan::districtCount() const {
    return mDistricts.size();
}

eAIDistrict& eAICityPlan::district(const int id) {
    return mDistricts[id];
}

void eAICityPlan::addDistrict(const eAIDistrict& a) {
    mDistricts.push_back(a);
}

int eAICityPlan::districtCost(eGameBoard& board, const int id) const {
    int result = 0;
    const auto d = mDistricts[id];
    const auto pid = board.cityIdToPlayerId(mCid);
    const auto diff = board.difficulty(pid);
    for(const auto& b : d.fBuildings) {
        result += eDifficultyHelpers::buildingCost(diff, b.fType);
    }
    return result;
}

int eAICityPlan::nextDistrictId() const {
    const int iMax = mDistricts.size();
    for(int i = 0; i < iMax; i++) {
        const bool b = eVectorHelpers::contains(mBuiltDistrics, i);
        if(b) continue;
        return i;
    }
    return -1;
}

int eAICityPlan::lastBuiltDistrictId() const {
    const int iMax = mDistricts.size() - 1;
    for(int i = iMax; i >= 0; i--) {
        const bool b = eVectorHelpers::contains(mBuiltDistrics, i);
        if(b) return i;
    }
    return -1;
}

void eAICityPlan::buildDistrict(eGameBoard& board, const int id) {
    const bool c = districtBuilt(id);
    if(!c) mBuiltDistrics.push_back(id);
    const auto pid = board.cityIdToPlayerId(mCid);
    mDistricts[id].build(board, pid, mCid, false);
}

void eAICityPlan::buildAllDistricts(eGameBoard& board) {
    const int iMax = mDistricts.size();
    for(int i = 0; i < iMax; i++) {
        buildDistrict(board, i);
    }
}

void eAICityPlan::rebuildDistricts(eGameBoard& board) {
    const auto pid = board.cityIdToPlayerId(mCid);
    for(const int id : mBuiltDistrics) {
        mDistricts[id].build(board, pid, mCid, false);
    }
}

bool eAICityPlan::connectDistricts(const int id1, const int id2,
                                   ePoints& points) {
    if(!districtBuilt(id1)) return false;
    if(!districtBuilt(id2)) return false;

    const auto& d1 = mDistricts[id1];
    int r1x;
    int r1y;
    const bool road1 = d1.road(r1x, r1y);
    if(!road1) return false;

    const auto& d2 = mDistricts[id2];
    int r2x;
    int r2y;
    const bool road2 = d2.road(r2x, r2y);
    if(!road2) return false;

    points.push_back({SDL_Point{r1x, r1y}, SDL_Point{r2x, r2y}});

    return true;
}

void eAICityPlan::connectAllBuiltDistricts(eGameBoard& board) {
    ePoints points;

    const int ijMax = mBuiltDistrics.size();
    for(int i = 0; i < ijMax; i++) {
        for(int j = i + 1; j < ijMax; j++) {
            connectDistricts(i, j, points);
        }
    }

    if(points.empty()) return;
    auto& tp = board.threadPool();
    tp.scheduleDataUpdate();
    const auto pid = board.cityIdToPlayerId(mCid);
    const auto task = new eAIConnectWithRoadTask(
                          board, points, pid, mCid);
    tp.queueTask(task);
}

bool eAICityPlan::districtBuilt(const int id) const {
    return eVectorHelpers::contains(mBuiltDistrics, id);
}

void eAICityPlan::editorDisplayBuildings(eGameBoard& board) {
    const int tmpi = board.editorCurrentDistrict();
    int i = 0;
    for(const auto& d : mDistricts) {
        board.setEditorCurrentDistrict(i++);
        const auto pid = board.cityIdToPlayerId(mCid);
        d.build(board, pid, mCid, true);
    }
    board.setEditorCurrentDistrict(tmpi);
}

void eAICityPlan::read(eReadStream& src) {
    src >> mCid;

    int bd;
    src >> bd;
    for(int i = 0; i < bd; i++) {
        int d;
        src >> d;
        mBuiltDistrics.push_back(d);
    }

    int ds;
    src >> ds;
    for(int i = 0; i < ds; i++) {
        auto& d = mDistricts.emplace_back();
        d.read(src);
    }
}

void eAICityPlan::write(eWriteStream& dst) const {
    dst << mCid;

    dst << mBuiltDistrics.size();
    for(const int d : mBuiltDistrics) {
        dst << d;
    }

    dst << mDistricts.size();
    for(const auto& d : mDistricts) {
        d.write(dst);
    }
}
