#include "ai-city-plan.h"

#include "engine/game-board.h"

#include "evectorhelpers.h"

#include "engine/epathfinder.h"

#include "etilehelper.h"
#include "fileIO/esavearchive.h"

AICityPlan::AICityPlan(const eCityId cid) :
    mCid(cid) {}

void AICityPlan::addScheduledBuilding(
        const int did, const SDL_Rect& bRect) {
    const auto& d = mDistricts[did];
    for(const auto& b : d.fBuildings) {
        const auto bbRect = b.fRect;
        if(!SDL_RectEquals(&bRect, &bbRect)) continue;
        addScheduledBuilding(did, b);
        break;
    }
}

void AICityPlan::addScheduledBuilding(const int did, const AIBuilding& b) {
    mScheduledBuildings.push_back({did, b});
}

int AICityPlan::districtCount() const {
    return mDistricts.size();
}

AIDistrict& AICityPlan::district(const int id) {
    return mDistricts[id];
}

void AICityPlan::addDistrict(const AIDistrict& a) {
    mDistricts.push_back(a);
}

int AICityPlan::districtCost(GameBoard& board, const int id,
                              int* const marble) const {
    int result = 0;
    const auto d = mDistricts[id];
    const auto pid = board.cityIdToPlayerId(mCid);
    const auto diff = board.difficulty(pid);
    if(marble) *marble = 0;
    for(const auto& b : d.fBuildings) {
        result += DifficultyHelpers::buildingCost(diff, b.fType);
        if(marble) *marble += eBuilding::sInitialMarbleCost(b.fType);
    }
    return result;
}

int AICityPlan::nextDistrictId() const {
    const int iMax = mDistricts.size() - 1;
    if(mLastBuildDistrict >= iMax) return -1;
    return mLastBuildDistrict + 1;
}

int AICityPlan::lastBuiltDistrictId() const {
    return mLastBuildDistrict;
}

bool AICityPlan::buildNextDistrict(GameBoard& board) {
    const int id = nextDistrictId();
    if(id == -1) return false;
    mLastBuildDistrict = id;
    const auto pid = board.cityIdToPlayerId(mCid);
    eDistrictIdTmp idTmp(board);
    board.setCurrentDistrictId(id);
    const auto scha = [this, id](const AIBuilding& b) {
        addScheduledBuilding(id, b);
    };
    mDistricts[id].build(board, pid, mCid, false, scha);
    return true;
}

void AICityPlan::buildAllDistricts(GameBoard& board) {
    const int iMax = mDistricts.size();
    for(int i = 0; i < iMax; i++) {
        buildNextDistrict(board);
    }
}

void AICityPlan::buildScheduled(GameBoard& board) {
    eDistrictIdTmp idTmp(board);
    const auto pid = board.cityIdToPlayerId(mCid);
    for(int i = 0; i < int(mScheduledBuildings.size()); i++) {
        const auto& bp = mScheduledBuildings[i];
        const int did = bp.first;
        const auto& b = bp.second;
        board.setCurrentDistrictId(did);
        const bool r = AIDistrict::sBuild(board, pid, mCid, false, b);
        if(r) {
            mScheduledBuildings.erase(mScheduledBuildings.begin() + i);
            i--;
        }
    }
}

bool AICityPlan::districtBuilt(const int id) const {
    return id <= mLastBuildDistrict;
}

void AICityPlan::editorDisplayBuildings(GameBoard& board) {
    eDistrictIdTmp idTmp(board);
    int i = 0;
    for(const auto& d : mDistricts) {
        board.setCurrentDistrictId(i++);
        const auto pid = board.cityIdToPlayerId(mCid);
        d.build(board, pid, mCid, true);
    }
}

void AICityPlan::serialize(eSaveArchive& ar) {
    ar.field("cityId", mCid);
    ar.field("lastBuildDistrict", mLastBuildDistrict);

    ar.arrayField("districts", mDistricts,
        [](eSaveArchive& itemAr, AIDistrict& d) { d.serialize(itemAr); });

    ar.arrayField("scheduledBuildings", mScheduledBuildings,
        [](eSaveArchive& itemAr, std::pair<int, AIBuilding>& bp) {
            itemAr.field("districtId", bp.first);
            bp.second.serialize(itemAr);
        });
}
