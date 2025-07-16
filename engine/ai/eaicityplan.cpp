#include "eaicityplan.h"

#include "engine/egameboard.h"

#include "evectorhelpers.h"

#include "engine/epathfinder.h"

#include "etilehelper.h"

eAICityPlan::eAICityPlan(const eCityId cid) :
    mCid(cid) {}

void eAICityPlan::addScheduledBuilding(
        const int did, const SDL_Rect& bRect) {
    const auto& d = mDistricts[did];
    for(const auto& b : d.fBuildings) {
        const auto bbRect = b.fRect;
        if(!SDL_RectEquals(&bRect, &bbRect)) continue;
        addScheduledBuilding(did, b);
        break;
    }
}

void eAICityPlan::addScheduledBuilding(const int did, const eAIBuilding& b) {
    mScheduledBuildings.push_back({did, b});
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

int eAICityPlan::districtCost(eGameBoard& board, const int id,
                              int* const marble) const {
    int result = 0;
    const auto d = mDistricts[id];
    const auto pid = board.cityIdToPlayerId(mCid);
    const auto diff = board.difficulty(pid);
    if(marble) *marble = 0;
    for(const auto& b : d.fBuildings) {
        result += eDifficultyHelpers::buildingCost(diff, b.fType);
        if(marble) *marble += eBuilding::sInitialMarbleCost(b.fType);
    }
    return result;
}

int eAICityPlan::nextDistrictId() const {
    const int iMax = mDistricts.size() - 1;
    if(mLastBuildDistrict >= iMax) return -1;
    return mLastBuildDistrict + 1;
}

int eAICityPlan::lastBuiltDistrictId() const {
    return mLastBuildDistrict;
}

bool eAICityPlan::buildNextDistrict(eGameBoard& board) {
    const int id = nextDistrictId();
    if(id == -1) return false;
    mLastBuildDistrict = id;
    const auto pid = board.cityIdToPlayerId(mCid);
    eDistrictIdTmp idTmp(board);
    board.setCurrentDistrictId(id);
    const auto scha = [this, id](const eAIBuilding& b) {
        addScheduledBuilding(id, b);
    };
    mDistricts[id].build(board, pid, mCid, false, scha);
    return true;
}

void eAICityPlan::buildAllDistricts(eGameBoard& board) {
    const int iMax = mDistricts.size();
    for(int i = 0; i < iMax; i++) {
        buildNextDistrict(board);
    }
}

void eAICityPlan::buildScheduled(eGameBoard& board) {
    eDistrictIdTmp idTmp(board);
    const auto pid = board.cityIdToPlayerId(mCid);
    for(int i = 0; i < int(mScheduledBuildings.size()); i++) {
        const auto& bp = mScheduledBuildings[i];
        const int did = bp.first;
        const auto& b = bp.second;
        board.setCurrentDistrictId(did);
        const bool r = eAIDistrict::sBuild(board, pid, mCid, false, b);
        if(r) {
            mScheduledBuildings.erase(mScheduledBuildings.begin() + i);
            i--;
        }
    }
}

bool eAICityPlan::districtBuilt(const int id) const {
    return id <= mLastBuildDistrict;
}

void eAICityPlan::editorDisplayBuildings(eGameBoard& board) {
    eDistrictIdTmp idTmp(board);
    int i = 0;
    for(const auto& d : mDistricts) {
        board.setCurrentDistrictId(i++);
        const auto pid = board.cityIdToPlayerId(mCid);
        d.build(board, pid, mCid, true);
    }
}

void eAICityPlan::read(eReadStream& src) {
    src >> mCid;

    src >> mLastBuildDistrict;

    int ds;
    src >> ds;
    for(int i = 0; i < ds; i++) {
        auto& d = mDistricts.emplace_back();
        d.read(src);
    }

    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        int did;
        src >> did;
        eAIBuilding b;
        b.read(src);
        mScheduledBuildings.push_back({did, b});
    }
}

void eAICityPlan::write(eWriteStream& dst) const {
    dst << mCid;

    dst << mLastBuildDistrict;

    dst << mDistricts.size();
    for(const auto& d : mDistricts) {
        d.write(dst);
    }

    dst << mScheduledBuildings.size();
    for(const auto& bp : mScheduledBuildings) {
        dst << bp.first;
        bp.second.write(dst);
    }
}
