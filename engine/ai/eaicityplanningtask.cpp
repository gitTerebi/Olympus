#include "eaicityplanningtask.h"

#include "buildings/ebuilding.h"
#include "engine/eresourcetype.h"
#include "engine/epatrolguide.h"
#include "etilehelper.h"
#include "engine/thread/ethreadboard.h"
#include "eaicityplan.h"
#include "engine/boardData/eheatmaptask.h"
#include "engine/boardData/eheatmapdivisor.h"
#include "buildings/eheatgetters.h"
#include "engine/egameboard.h"

struct eAITile {
    eBuildingType fBuilding = eBuildingType::none;
    bool fMaintanance = false;
    bool fSecurity = false;
    bool fHealth = false;
    bool fTax = false;
    bool fPodium = false;
    bool fGymnasium = false;
    bool fTheater = false;
    bool fStadium = false;
    bool fAgora = false;
};

struct eAIBoard {
    eAITile* tile(const int dx, const int dy) {
        if(dx < 0) return nullptr;
        if(dy < 0) return nullptr;
        if(dx >= fW) return nullptr;
        if(dy >= fH) return nullptr;
        return &fTiles[dx][dy];
    }

    void initialize(const int w, const int h) {
        fW = w;
        fH = h;
        fTiles.clear();
        for(int x = 0; x < w; x++) {
            auto& row = fTiles.emplace_back();
            for(int y = 0; y < h; y++) {
                row.emplace_back();
            }
        }
    }

    int fW = 0;
    int fH = 0;
    std::vector<std::vector<eAITile>> fTiles;
};

bool gBuildableTile(eAIBoard& aiBoard,
                    const int dx, const int dy,
                    const bool isRoad) {
    const auto tile = aiBoard.tile(dx, dy);
    if(!tile) return false;
    const auto btype = tile->fBuilding;
    if(btype != eBuildingType::none) {
        if(btype == eBuildingType::road) {
            if(!isRoad) return false;
        } else {
            return false;
        }
    }
    return true;
}

bool gBuildableTile(eThreadBoard& board,
                    const int dx, const int dy,
                    const eCityId cid, const bool isRoad) {
    const auto btile = board.dtile(dx, dy);
    if(!btile || btile->cityId() != cid) {
        return false;
    }
    const auto terr = btile->terrain();
    if(!static_cast<bool>(terr & eTerrain::buildableAfterClear)) {
        return false;
    }
    const auto btype = btile->underBuildingType();
    if(btype != eBuildingType::none) {
        if(btype == eBuildingType::road) {
            if(!isRoad) return false;
        } else {
            return false;
        }
    }
    const bool e = btile->isElevationTile();
    if(e) {
        if(isRoad) {
            return btile->walkableElev();
        } else {
            return false;
        }
    }
    return true;
}

bool gBuildableTile(eThreadBoard& board, eAIBoard& aiBoard,
                    const int dx, const int dy,
                    const eCityId cid, const bool isRoad) {
    if(!gBuildableTile(aiBoard, dx, dy, isRoad)) return false;
    return gBuildableTile(board, dx, dy, cid, isRoad);
}

struct eRoadBoard {
    char* tile(const int dx, const int dy) {
        if(dx < 0) return nullptr;
        if(dy < 0) return nullptr;
        if(dx >= fW) return nullptr;
        if(dy >= fH) return nullptr;
        return &fTiles[dx][dy];
    }

    void initialize(const int w, const int h) {
        fW = w;
        fH = h;
        fTiles.clear();
        for(int x = 0; x < w; x++) {
            auto& row = fTiles.emplace_back();
            for(int y = 0; y < h; y++) {
                row.emplace_back() = 0;
            }
        }
    }

    bool hasRoad(const int dx, const int dy) {
        const auto t = tile(dx, dy);
        if(!t) return false;
        return *t == 1;
    }

    bool isCorner(const int x, const int y,
                  const eDiagonalOrientation o) {
        int x1 = x;
        int y1 = y;
        switch(o) {
        case eDiagonalOrientation::topRight: {
            y1--;
        } break;
        case eDiagonalOrientation::bottomRight: {
            x1++;
        } break;
        case eDiagonalOrientation::bottomLeft: {
            y1++;
        } break;
        case eDiagonalOrientation::topLeft: {
            x1--;
        } break;
        }

        int dx;
        int dy;
        eTileHelper::tileIdToDTileId(x1, y1, dx, dy);
        const bool r = hasRoad(dx, dy);
        if(!r) return false;

        int n = 0;

        {
            const int x2 = x - 1;
            const int y2 = y;
            int dx;
            int dy;
            eTileHelper::tileIdToDTileId(x2, y2, dx, dy);
            const bool r = hasRoad(dx, dy);
            if(r) n++;
        }

        {
            const int x3 = x + 1;
            const int y3 = y;
            int dx;
            int dy;
            eTileHelper::tileIdToDTileId(x3, y3, dx, dy);
            const bool r = hasRoad(dx, dy);
            if(r) n++;
        }

        {
            const int x4 = x;
            const int y4 = y - 1;
            int dx;
            int dy;
            eTileHelper::tileIdToDTileId(x4, y4, dx, dy);
            const bool r = hasRoad(dx, dy);
            if(r) n++;
        }

        {
            const int x5 = x;
            const int y5 = y + 1;
            int dx;
            int dy;
            eTileHelper::tileIdToDTileId(x5, y5, dx, dy);
            const bool r = hasRoad(dx, dy);
            if(r) n++;
        }

        return n < 3;
    }

    int fW = 0;
    int fH = 0;
    std::vector<std::vector<char>> fTiles;
};

struct eAICBuilding {
    eBuildingType fType;
    eResourceType fGet = static_cast<eResourceType>(0);
    eResourceType fEmpty = static_cast<eResourceType>(0);
    eResourceType fAccept = static_cast<eResourceType>(0);
    std::vector<int> fGuideIds;
    eDiagonalOrientation fO = eDiagonalOrientation::topLeft;

    std::vector<int> fGuidesTmp;
    SDL_Rect fRectTmp;
};

struct eAIRoadPath {
    int xEnd() const {
        switch(fO) {
        case eDiagonalOrientation::topRight:
            return fX;
        case eDiagonalOrientation::bottomRight:
            return fX + fLen - 1;
        case eDiagonalOrientation::bottomLeft:
            return fX;
        case eDiagonalOrientation::topLeft:
            return fX - fLen + 1;
        }
    }

    int yEnd() const {
        switch(fO) {
        case eDiagonalOrientation::topRight:
            return fY - fLen + 1;
        case eDiagonalOrientation::bottomRight:
            return fY;
        case eDiagonalOrientation::bottomLeft:
            return fY + fLen - 1;
        case eDiagonalOrientation::topLeft:
            return fY;
        }
    }

    int minX() const {
        return std::min(fX, xEnd());
    }

    int maxX() const {
        return std::max(fX, xEnd());
    }

    int minY() const {
        return std::min(fY, yEnd());
    }

    int maxY() const {
        return std::max(fY, yEnd());
    }

    void coordinatesAt(int displ, int& x, int& y) {
        displ = std::max(0, std::min(fLen - 1, displ));
        switch(fO) {
        case eDiagonalOrientation::topRight:
            x = fX;
            y = fY - displ;
            break;
        case eDiagonalOrientation::bottomRight:
            x = fX + displ;
            y = fY;
            break;
        case eDiagonalOrientation::bottomLeft:
            x = fX;
            y = fY + displ;
            break;
        case eDiagonalOrientation::topLeft:
            x = fX - displ;
            y = fY;
            break;
        }
    }

    void updateCoordinates() {
        for(auto& b : fBranches) {
            coordinatesAt(b.fDisplacement, b.fX, b.fY);
            b.updateCoordinates();
        }
    }

    void allBranches(std::vector<eAIRoadPath*>& result,
                     const bool skipCycleBranches) {
        result.push_back(this);
        if(fType == eType::cycle && skipCycleBranches) return;
        for(auto& b : fBranches) {
            b.allBranches(result, skipCycleBranches);
        }
    }

    void updateCycleBranches() {
        fBranches.clear();

        auto& b0 = fBranches.emplace_back();
        b0.fX = fX;
        b0.fY = fY;
        b0.fDisplacement = 0;
        b0.fLen = fLen;
        b0.fO = fO;

        auto& b1 = b0.fBranches.emplace_back();
        b1.fX = b0.xEnd();
        b1.fY = b0.yEnd();
        b1.fDisplacement = b0.fLen;
        b1.fLen = fWidth;
        switch(b0.fO) {
        case eDiagonalOrientation::topRight:
            b1.fO = eDiagonalOrientation::bottomRight;
            break;
        case eDiagonalOrientation::bottomRight:
            b1.fO = eDiagonalOrientation::bottomLeft;
            break;
        case eDiagonalOrientation::bottomLeft:
            b1.fO = eDiagonalOrientation::topLeft;
            break;
        case eDiagonalOrientation::topLeft:
            b1.fO = eDiagonalOrientation::topRight;
            break;
        }

        auto& b2 = b1.fBranches.emplace_back();
        b2.fX = b1.xEnd();
        b2.fY = b1.yEnd();
        b2.fDisplacement = b1.fLen;
        b2.fLen = b0.fLen;
        b2.fO = !b0.fO;

        auto& b3 = b2.fBranches.emplace_back();
        b3.fX = b2.xEnd();
        b3.fY = b2.yEnd();
        b3.fDisplacement = b2.fLen;
        b3.fLen = fWidth;
        b3.fO = !b1.fO;
    }

    eAIRoadPath& branchAt(int id) {
        if(id <= 0) return *this;
        id--;
        for(auto& b : fBranches) {
            const int n = b.totalBranchesCount();
            if(id < n) {
                return b.branchAt(id);
            } else {
                id -= n;
            }
        }
        return *this;
    }

    eAIRoadPath& randomBranch() {
        const int n = totalBranchesCount();
        const int id = eRand::rand() % n;
        return branchAt(id);
    }

    int totalBranchesCount() const {
        int result = 1;
        for(const auto& b : fBranches) {
            result += b.totalBranchesCount();
        }
        return result;
    }

    int totalBranchesLen() const {
        int result = fLen;
        for(const auto& b : fBranches) {
            result += b.totalBranchesLen();
        }
        return result;
    }

    enum class eType {
        branch,
        cycle
    };

    struct eCycleTurn {
        int fDisplacement;
        eDiagonalOrientation fO;
    };

    eType fType = eType::branch;
    int fX;
    int fY;
    int fDisplacement;
    eDiagonalOrientation fO;
    int fLen;

    std::vector<eAIRoadPath> fBranches;

    // only for cycle
    int fWidth;
    std::vector<eCycleTurn> fTurns;
};

bool gHasRoad(const int x, const int y,
              eRoadBoard& roadBoard) {
    int dx;
    int dy;
    eTileHelper::tileIdToDTileId(x, y, dx, dy);
    const auto tile = roadBoard.tile(dx, dy);
    if(!tile) return false;
    return *tile == 1;
}

bool gHasRoad(const int xMin, const int yMin,
              const int xMax, const int yMax,
              eRoadBoard& roadBoard) {
    for(int x = xMin; x <= xMax; x++) {
        for(int y = yMin; y <= yMax; y++) {
            const bool r = gHasRoad(x, y, roadBoard);
            if(r) return true;
        }
    }
    return false;
}

bool gHasRoad(const int xMin, const int yMin,
              const int xMax, const int yMax,
              eRoadBoard& roadBoard,
              const eDiagonalOrientation o) {
    bool found = false;
    for(int x = xMin; x <= xMax; x++) {
        for(int y = yMin; y <= yMax; y++) {
            const bool r = gHasRoad(x, y, roadBoard);
            if(r) {
                found = true;
                const bool c = roadBoard.isCorner(x, y, o);
                if(c) return false;
            }
        }
    }
    return found;
}

bool gNextToRoad(const int xMin, const int yMin,
                 const int xMax, const int yMax,
                 eRoadBoard& roadBoard,
                 std::vector<eDiagonalOrientation>* const o) {
    const bool r1 = gHasRoad(xMin, yMin - 1, xMax, yMin - 1, roadBoard,
                             eDiagonalOrientation::topRight);
    if(r1) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::topRight);
    }
    const bool r2 = gHasRoad(xMin, yMax + 1, xMax, yMax + 1, roadBoard,
                             eDiagonalOrientation::bottomLeft);
    if(r2) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::bottomLeft);
    }
    const bool r3 = gHasRoad(xMin - 1, yMin, xMin - 1, yMax, roadBoard,
                             eDiagonalOrientation::topLeft);
    if(r3) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::topLeft);
    }
    const bool r4 = gHasRoad(xMax + 1, yMin, xMax + 1, yMax, roadBoard,
                             eDiagonalOrientation::bottomRight);
    if(r4) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::bottomRight);
    }
    return o ? !o->empty() : false;
}

struct eAICDistrict {

    void addToCityPlan(eAICityPlan& result) {
        eAIDistrict district;

        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads, false);
        for(const auto& road : allRoads) {
            const int xMin = road->minX();
            const int xMax = road->maxX();
            const int yMin = road->minY();
            const int yMax = road->maxY();

            for(int x = xMin; x <= xMax; x++) {
                for(int y = yMin; y <= yMax; y++) {
                    eAIBuilding b;
                    b.fType = eBuildingType::road;
                    b.fRect = SDL_Rect{x, y, 1, 1};
                    district.addBuilding(b);
                }
            }
        }

        for(const auto& b : fBuildings) {
            const auto& rect = b.fRectTmp;
            if(rect.w == 0) continue;
            eAIBuilding bb;
            bb.fType = b.fType;
            bb.fRect = b.fRectTmp;
            district.addBuilding(bb);
        }

        for(const auto& b : fTmpBuildings) {
            const auto& rect = b.fRectTmp;
            if(rect.w == 0) continue;
            eAIBuilding bb;
            bb.fType = b.fType;
            bb.fRect = b.fRectTmp;
            district.addBuilding(bb);
        }

        result.addDistrict(district);
    }

    bool validRoad(eThreadBoard& board,
                   const int xMin, const int xMax,
                   const int yMin, const int yMax) {
        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                int dx;
                int dy;
                eTileHelper::tileIdToDTileId(x, y, dx, dy);
                const bool b = gBuildableTile(board, dx, dy, fCid, true);
                if(!b) return false;
            }
        }
        return true;
    }

    bool validRoad(eThreadBoard& board,
                   const eAIRoadPath& road) {
        if(road.fType == eAIRoadPath::eType::branch) {
            const int xMin = road.minX();
            const int xMax = road.maxX();
            const int yMin = road.minY();
            const int yMax = road.maxY();

            const bool r = validRoad(board, xMin, xMax, yMin, yMax);
            if(!r) return false;
        }
        for(const auto& b : road.fBranches) {
            const bool r = validRoad(board, b);
            if(!r) return false;
        }
        return true;
    }

    bool changeRoad(eThreadBoard& board) {
        enum class eType {
            add,
            remove,
            resize,
            changeWidth, // for cycle
            move
        };

        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads, true);
        const int srcRoadId = eRand::rand() % allRoads.size();
        auto& srcRoad = *allRoads[srcRoadId];
        std::vector<eType> types;
        if(srcRoad.fType == eAIRoadPath::eType::cycle) {
            types = {eType::changeWidth, eType::resize, eType::move};
        } else if(srcRoad.fLen <= 0) {
            types = {eType::resize, eType::move};
        } else if(srcRoad.fBranches.empty()) {
            types = {eType::add, eType::resize, eType::move};
        } else {
            types = {eType::add, eType::remove, eType::resize, eType::move};
        }
        if(types.empty()) return false;
        const auto type = types[eRand::rand() % types.size()];
        switch(type) {
        case eType::add: {
            eAIRoadPath newRoad;
            const int type = eRand::rand() % 2;
            if(type) { // branch
                newRoad.fType = eAIRoadPath::eType::branch;
                newRoad.fDisplacement = eRand::rand() % srcRoad.fLen;
                newRoad.fLen = 2 + (eRand::rand() % 10);
                srcRoad.coordinatesAt(newRoad.fDisplacement, newRoad.fX, newRoad.fY);
                const int oi = eRand::rand() % 2;
                switch(srcRoad.fO) {
                case eDiagonalOrientation::topRight:
                case eDiagonalOrientation::bottomLeft: {
                    if(oi) {
                        newRoad.fO = eDiagonalOrientation::topLeft;
                    } else {
                        newRoad.fO = eDiagonalOrientation::bottomRight;
                    }
                } break;
                case eDiagonalOrientation::topLeft:
                case eDiagonalOrientation::bottomRight:
                default: {
                    if(oi) {
                        newRoad.fO = eDiagonalOrientation::topRight;
                    } else {
                        newRoad.fO = eDiagonalOrientation::bottomLeft;
                    }
                } break;
                }
            } else { // cycle
                newRoad.fType = eAIRoadPath::eType::cycle;
                newRoad.fDisplacement = eRand::rand() % srcRoad.fLen;
                newRoad.fLen = 2 + (eRand::rand() % 10);
                newRoad.fWidth = 2 + (eRand::rand() % 10);
                srcRoad.coordinatesAt(newRoad.fDisplacement, newRoad.fX, newRoad.fY);
                const int oi = eRand::rand() % 2;
                switch(srcRoad.fO) {
                case eDiagonalOrientation::topRight:
                case eDiagonalOrientation::bottomLeft: {
                    if(oi) {
                        newRoad.fO = eDiagonalOrientation::topLeft;
                    } else {
                        newRoad.fO = eDiagonalOrientation::bottomRight;
                    }
                } break;
                case eDiagonalOrientation::topLeft:
                case eDiagonalOrientation::bottomRight:
                default: {
                    if(oi) {
                        newRoad.fO = eDiagonalOrientation::topRight;
                    } else {
                        newRoad.fO = eDiagonalOrientation::bottomLeft;
                    }
                } break;
                }
                newRoad.updateCycleBranches();
            }

            const bool r = validRoad(board, newRoad);
            if(!r) return false;
            srcRoad.fBranches.push_back(newRoad);
            return true;
        } break;
        case eType::remove: {
            auto& bs = srcRoad.fBranches;
            if(bs.empty()) return false;
            const int toDelete = eRand::rand() % bs.size();
            bs.erase(bs.begin() + toDelete);
            return true;
        } break;
        case eType::resize: {
            eAIRoadPath tmp = srcRoad;
            int by = 0;
            while(by == 0) {
                by = 2 - (eRand::rand() % 5);
            }
            tmp.fLen = std::max(1, tmp.fLen + by);
            if(tmp.fType == eAIRoadPath::eType::cycle) {
                tmp.updateCycleBranches();
            } else {
                tmp.updateCoordinates();
            }
            const bool r = validRoad(board, tmp);
            if(!r) return false;
            srcRoad = tmp;
            return true;
        } break;
        case eType::changeWidth: {
            if(srcRoad.fType != eAIRoadPath::eType::cycle) return false;
            eAIRoadPath tmp = srcRoad;
            int by = 0;
            while(by == 0) {
                by = 2 - (eRand::rand() % 5);
            }
            tmp.fWidth = std::max(1, tmp.fWidth + by);
            tmp.updateCycleBranches();
            const bool r = validRoad(board, tmp);
            if(!r) return false;
            srcRoad = tmp;
            return true;
        } break;
        case eType::move: {
            const int toMoveId = eRand::rand() % allRoads.size();
            auto& toMove = *allRoads[toMoveId];
            eAIRoadPath tmp = toMove;
            tmp.fDisplacement += 2 - (eRand::rand() % 5);
            srcRoad.coordinatesAt(tmp.fDisplacement, tmp.fX, tmp.fY);
            tmp.updateCoordinates();
            const bool r = validRoad(board, tmp);
            if(!r) return false;
            toMove = tmp;
            return true;
        } break;
        }

        return false;
    }

    bool move(eThreadBoard& board,
              const int byX, const int byY) {
        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads, false);
        for(const auto road : allRoads) {
            if(road->fType == eAIRoadPath::eType::cycle) continue;
            const int xMin = road->minX() + byX;
            const int xMax = road->maxX() + byX;
            const int yMin = road->minY() + byY;
            const int yMax = road->maxY() + byY;
            const bool r = validRoad(board, xMin, xMax, yMin, yMax);
            if(!r) return false;
        }
        for(const auto road : allRoads) {
            road->fX += byX;
            road->fY += byY;
        }
        return true;
    }

    bool swapBuildings(const int b1, const int b2) {
        if(b1 == b2) return false;
        std::swap(fBuildings[b1], fBuildings[b2]);
        return true;
    }

    bool swapBuildings() {
        if(fBuildings.size() < 2) return false;
        const int b1 = eRand::rand() % fBuildings.size();
        const int b2 = eRand::rand() % fBuildings.size();
        return swapBuildings(b1, b2);
    }

    eAICBuilding& addBuilding(const eBuildingType type) {
        auto& result = fBuildings.emplace_back();
        result.fType = type;
        return result;
    }

    bool placeBuilding(const SDL_Rect& roadsBRect,
                       eThreadBoard& board,
                       eAIBoard& aiBoard,
                       eRoadBoard& roadBoard,
                       eAICBuilding& b,
                       SDL_Rect& buildingsBRect) {
        bool needsFertile = false;
        bool useAvenue = false;
        int w;
        int h;
        int findW;
        int findH;
        switch(b.fType) {
        case eBuildingType::commonHouse:
            useAvenue = true;
        case eBuildingType::maintenanceOffice:
        case eBuildingType::taxOffice:
        case eBuildingType::podium:
        case eBuildingType::watchPost:
        case eBuildingType::fountain: {
            w = 2;
            h = 2;
        } break;
        case eBuildingType::eliteHousing: {
            useAvenue = true;
            w = 4;
            h = 4;
        } break;
        case eBuildingType::dramaSchool:
        case eBuildingType::college:
        case eBuildingType::gymnasium: {
            w = 3;
            h = 3;
        } break;
        case eBuildingType::theater: {
            w = 5;
            h = 5;
        } break;
        case eBuildingType::onionsFarm:
        case eBuildingType::carrotsFarm:
        case eBuildingType::wheatFarm: {
            w = 3;
            h = 3;
            needsFertile = true;
        } break;
        case eBuildingType::granary: {
            w = 4;
            h = 4;
        } break;
        case eBuildingType::warehouse: {
            w = 3;
            h = 3;
        } break;
        case eBuildingType::commonAgora: {
            if(b.fO == eDiagonalOrientation::topLeft ||
               b.fO == eDiagonalOrientation::bottomRight) {
                w = 3;
                h = 6;
                findW = 2;
                findH = 0;
            } else {
                w = 6;
                h = 3;
                findW = 0;
                findH = 2;
            }
        } break;
        case eBuildingType::grandAgora: {
            if(b.fO == eDiagonalOrientation::topLeft ||
               b.fO == eDiagonalOrientation::bottomRight) {
                w = 5;
                h = 6;
                findW = 2;
                findH = 0;
            } else {
                w = 6;
                h = 5;
                findW = 0;
                findH = 2;
            }
        } break;
        }

        if(b.fType != eBuildingType::commonAgora &&
           b.fType != eBuildingType::grandAgora) {
            findW = w;
            findH = h;
        }

        const int xMin1 = roadsBRect.x - findW;
        const int xMax1 = roadsBRect.x + roadsBRect.w;
        const int yMin1 = roadsBRect.y - findH;
        const int yMax1 = roadsBRect.y + roadsBRect.h;
        for(int x1 = xMin1; x1 <= xMax1; x1++) {
            for(int y1 = yMin1; y1 <= yMax1; y1++) {
                int xMin = x1;
                int xMax = x1 + w - 1;
                int yMin = y1;
                int yMax = y1 + h - 1;
                int totalXMin = xMin;
                int totalXMax = xMax;
                int totalYMin = yMin;
                int totalYMax = yMax;
                const bool isAgora = b.fType == eBuildingType::commonAgora ||
                                     b.fType == eBuildingType::grandAgora;
                const bool zero = b.fType == eBuildingType::commonAgora &&
                                  (b.fO == eDiagonalOrientation::bottomRight ||
                                   b.fO == eDiagonalOrientation::bottomLeft);
                const int allowedRoadX = isAgora ? (w == 6 ? __INT_MAX__ : (zero ? x1 : (x1 + 2))) : __INT_MAX__;
                const int allowedRoadY = isAgora ? (w == 6 ? (zero ? y1 : (y1 + 2)) : __INT_MAX__) : __INT_MAX__;
                std::vector<eDiagonalOrientation> os;
                if(isAgora) {

                } else {
                    const bool nextToRoad = gNextToRoad(xMin, yMin, xMax, yMax, roadBoard,
                                                        useAvenue ? &os : nullptr);
                    if(!nextToRoad) continue;
                    if(b.fType == eBuildingType::eliteHousing) {
                        for(const auto o : os) {
                            switch(o) {
                            case eDiagonalOrientation::bottomLeft:
                            case eDiagonalOrientation::topRight: {
                                xMin++;
                                xMax++;
                                totalXMax += 2;
                            } break;
                            case eDiagonalOrientation::topLeft:
                            case eDiagonalOrientation::bottomRight: {
                                yMin++;
                                yMax++;
                                totalYMax += 2;
                            } break;
                            }
                        }
                    }
                    if(useAvenue) {
                        for(const auto o : os) {
                            switch(o) {
                            case eDiagonalOrientation::topRight: {
                                yMin++;
                                yMax++;
                                totalYMax++;
                            } break;
                            case eDiagonalOrientation::bottomRight: {
                                xMin--;
                                xMax--;
                                totalXMin--;
                            } break;
                            case eDiagonalOrientation::bottomLeft: {
                                yMin--;
                                yMax--;
                                totalYMin--;
                            } break;
                            case eDiagonalOrientation::topLeft: {
                                xMin++;
                                xMax++;
                                totalXMax++;
                            } break;
                            }
                        }
                    }
                }
                bool ok = true;
                bool foundFertile = false;
                for(int x = totalXMin; x <= totalXMax; x++) {
                    for(int y = totalYMin; y <= totalYMax; y++) {
                        int dx;
                        int dy;
                        eTileHelper::tileIdToDTileId(x, y, dx, dy);
                        const auto tile = aiBoard.tile(dx, dy);
                        if(!tile) {
                            ok = false;
                            break;
                        }
                        const auto type = tile->fBuilding;
                        if(x == allowedRoadX || y == allowedRoadY) {
                            if(type != eBuildingType::road) {
                                ok = false;
                                break;
                            }
                        } else if(type != eBuildingType::none) {
                            ok = false;
                            break;
                        }
                        const auto vtile = board.dtile(dx, dy);
                        if(!vtile) {
                            ok = false;
                            break;
                        }
                        const auto ttype = vtile->underBuildingType();
                        if(ttype != eBuildingType::none) {
                            ok = false;
                            break;
                        }
                        const auto terr = vtile->terrain();
                        const bool f = static_cast<bool>(terr & eTerrain::fertile);
                        if(f) foundFertile = true;
                        const bool v = static_cast<bool>(terr & eTerrain::buildableAfterClear);
                        if(!v || vtile->isElevationTile()) {
                            ok = false;
                            break;
                        }
                    }
                    if(!ok) break;
                }
                if(needsFertile && !foundFertile) continue;
                if(!ok) continue;

                b.fRectTmp = SDL_Rect{xMin, yMin, w, h};
                for(int x = xMin; x <= xMax; x++) {
                    for(int y = yMin; y <= yMax; y++) {
                        int dx;
                        int dy;
                        eTileHelper::tileIdToDTileId(x, y, dx, dy);
                        const auto tile = aiBoard.tile(dx, dy);
                        const auto type = tile->fBuilding;
                        if(type == eBuildingType::road &&
                           (x == allowedRoadX || y == allowedRoadY)) {

                        } else {
                            tile->fBuilding = b.fType;
                        }
                    }
                }
                if(buildingsBRect.w == 0) {
                    buildingsBRect = b.fRectTmp;
                } else {
                    const auto tmp = buildingsBRect;
                    SDL_UnionRect(&b.fRectTmp, &tmp, &buildingsBRect);
                }

                if(b.fType == eBuildingType::commonHouse) {
                    for(const auto o : os) {
                        int xMinG = xMin;
                        int yMinG = yMin;
                        switch(o) {
                        case eDiagonalOrientation::topRight: {
                            yMinG += 2;
                        } break;
                        case eDiagonalOrientation::bottomRight: {
                            xMinG -= 2;
                        } break;
                        case eDiagonalOrientation::bottomLeft: {
                            yMinG -= 2;
                        } break;
                        case eDiagonalOrientation::topLeft: {
                            xMinG += 2;
                        } break;
                        }

                        const int xMaxG = xMinG + 1;
                        const int yMaxG = yMinG + 1;
                        bool ok = true;
                        for(int x = xMinG; x <= xMaxG; x++) {
                            for(int y = yMinG; y <= yMaxG; y++) {
                                int dx;
                                int dy;
                                eTileHelper::tileIdToDTileId(x, y, dx, dy);
                                const bool b = gBuildableTile(board, aiBoard, dx, dy, fCid, false);
                                if(!b) {
                                    ok = false;
                                    break;
                                }
                            }
                            if(!ok) break;
                        }
                        if(!ok) continue;

                        auto& b = fTmpBuildings.emplace_back();
                        const int par = (xMinG + yMinG) % 6;
                        if(par < 2) {
                            b.fType = eBuildingType::gazebo;
                        } else if(par < 4) {
                            b.fType = eBuildingType::flowerGarden;
                        } else if(par < 6) {
                            b.fType = eBuildingType::shellGarden;
                        }
                        b.fRectTmp = SDL_Rect{xMinG, yMinG, 2, 2};
                        for(int x = xMinG; x <= xMaxG; x++) {
                            for(int y = yMinG; y <= yMaxG; y++) {
                                int dx;
                                int dy;
                                eTileHelper::tileIdToDTileId(x, y, dx, dy);
                                const auto tile = aiBoard.tile(dx, dy);
                                tile->fBuilding = b.fType;
                            }
                        }
                        if(buildingsBRect.w == 0) {
                            buildingsBRect = b.fRectTmp;
                        } else {
                            const auto tmp = buildingsBRect;
                            SDL_UnionRect(&b.fRectTmp, &tmp, &buildingsBRect);
                        }
                    }
                } else if(b.fType == eBuildingType::eliteHousing) {
                    const auto place = [&](const eBuildingType type,
                                          const int xMinC, const int yMinC,
                                          const int xMaxC, const int yMaxC) {
                        for(int x = xMinC; x <= xMaxC; x++) {
                            for(int y = yMinC; y <= yMaxC; y++) {
                                int dx;
                                int dy;
                                eTileHelper::tileIdToDTileId(x, y, dx, dy);
                                const bool b = gBuildableTile(board, aiBoard, dx, dy, fCid, false);
                                if(!b) return;
                            }
                        }

                        auto& b = fTmpBuildings.emplace_back();
                        b.fType = type;
                        b.fRectTmp = SDL_Rect{xMinC, yMinC, 3, 3};
                        for(int x = xMinC; x <= xMaxC; x++) {
                            for(int y = yMinC; y <= yMaxC; y++) {
                                int dx;
                                int dy;
                                eTileHelper::tileIdToDTileId(x, y, dx, dy);
                                const auto tile = aiBoard.tile(dx, dy);
                                tile->fBuilding = b.fType;
                            }
                        }
                        if(buildingsBRect.w == 0) {
                            buildingsBRect = b.fRectTmp;
                        } else {
                            const auto tmp = buildingsBRect;
                            SDL_UnionRect(&b.fRectTmp, &tmp, &buildingsBRect);
                        }
                    };
                    {
                        int xMinC = xMin;
                        int yMinC = yMin;
                        for(const auto o : os) {
                            switch(o) {
                            case eDiagonalOrientation::topRight: {
                                xMinC -= 1;
                                yMinC += 4;
                            } break;
                            case eDiagonalOrientation::bottomRight: {
                                xMinC -= 3;
                                yMinC -= 1;
                            } break;
                            case eDiagonalOrientation::bottomLeft: {
                                xMinC -= 1;
                                yMinC -= 3;
                            } break;
                            case eDiagonalOrientation::topLeft: {
                                xMinC += 4;
                                yMinC -= 1;
                            } break;
                            }
                        }

                        const int xMaxC = xMinC + 2;
                        const int yMaxC = yMinC + 2;

                        place(eBuildingType::commemorative, xMinC, yMinC, xMaxC, yMaxC);
                    }
                    {
                        int xMinC = xMin;
                        int yMinC = yMin;
                        for(const auto o : os) {
                            switch(o) {
                            case eDiagonalOrientation::topRight: {
                                xMinC += 2;
                                yMinC += 4;
                            } break;
                            case eDiagonalOrientation::bottomRight: {
                                xMinC -= 3;
                                yMinC += 2;
                            } break;
                            case eDiagonalOrientation::bottomLeft: {
                                xMinC += 2;
                                yMinC -= 3;
                            } break;
                            case eDiagonalOrientation::topLeft: {
                                xMinC += 4;
                                yMinC += 2;
                            } break;
                            }
                        }

                        const int xMaxC = xMinC + 2;
                        const int yMaxC = yMinC + 2;

                        place(eBuildingType::hedgeMaze, xMinC, yMinC, xMaxC, yMaxC);
                    }
                }
                if(b.fType == eBuildingType::commonHouse ||
                   b.fType == eBuildingType::eliteHousing) {
                    for(int x = totalXMin; x <= totalXMax; x++) {
                        for(int y = totalYMin; y <= totalYMax; y++) {
                            int dx;
                            int dy;
                            eTileHelper::tileIdToDTileId(x, y, dx, dy);
                            const bool bb = gBuildableTile(board, aiBoard, dx, dy, fCid, false);
                            if(!bb) continue;
                            auto& b = fTmpBuildings.emplace_back();
                            const bool r = gHasRoad(x - 1, y - 1, x + 1, y + 1, roadBoard);
                            b.fType = r ? eBuildingType::avenue : eBuildingType::park;
                            b.fRectTmp = SDL_Rect{x, y, 1, 1};
                            const auto tile = aiBoard.tile(dx, dy);
                            tile->fBuilding = b.fType;
                            if(buildingsBRect.w == 0) {
                                buildingsBRect = b.fRectTmp;
                            } else {
                                const auto tmp = buildingsBRect;
                                SDL_UnionRect(&b.fRectTmp, &tmp, &buildingsBRect);
                            }
                        }
                    }
                }
                return true;
            }
        }
        return false;
    }

    void distributeBuildings(eThreadBoard& board,
                             eAIBoard& aiBoard) {
        SDL_Rect roadsBRect{0, 0, 0, 0};

        fRoads.updateCoordinates();
        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads, false);
        eRoadBoard roadBoard;
        roadBoard.initialize(aiBoard.fW, aiBoard.fH);
        for(const auto r : allRoads) {
            const int xMin = r->minX();
            const int xMax = r->maxX();
            const int yMin = r->minY();
            const int yMax = r->maxY();

            for(int x = xMin; x <= xMax; x++) {
                for(int y = yMin; y <= yMax; y++) {
                    if(roadsBRect.w == 0) {
                        roadsBRect.x = x;
                        roadsBRect.y = y;
                        roadsBRect.w = 1;
                        roadsBRect.h = 1;
                    } else {
                        const SDL_Rect a = roadsBRect;
                        const SDL_Rect b{x, y, 1, 1};
                        SDL_UnionRect(&a, &b, &roadsBRect);
                    }
                    int dx;
                    int dy;
                    eTileHelper::tileIdToDTileId(x, y, dx, dy);
                    const bool b = gBuildableTile(board, aiBoard, dx, dy, fCid, true);
                    if(!b) continue;
                    const auto tile = aiBoard.tile(dx, dy);
                    if(!tile) continue;
                    tile->fBuilding = eBuildingType::road;
                    {
                        const auto tile = roadBoard.tile(dx, dy);
                        *tile = 1;
                    }
                }
            }
        }

        fTmpBuildings.clear();
        SDL_Rect buildingsBRect{0, 0, 0, 0};
        for(auto& b : fBuildings) {
            b.fRectTmp = {0, 0, 0, 0};
            placeBuilding(roadsBRect, board, aiBoard, roadBoard, b, buildingsBRect);
        }
    }

    int grade(eThreadBoard& board, eAIBoard& aiBoard) const {
        int result = 0;

        result -= fRoads.totalBranchesCount();
        result -= fRoads.totalBranchesLen();

        result -= fBuildings.size();
        result -= fTmpBuildings.size();

        SDL_Rect bRect{0, 0, 0, 0};

        int placed = 0;

        for(const auto& b : fBuildings) {
            if(b.fRectTmp.w == 0) continue;
            placed++;

            if(bRect.w == 0) {
                bRect = b.fRectTmp;
            } else {
                const auto tmp = bRect;
                SDL_UnionRect(&b.fRectTmp, &tmp, &bRect);
            }

            const auto type = b.fType;

            bool maintanance = false;
            bool security = false;
            bool health = false;
            bool tax = false;
            bool podium = false;
            bool gymnasium = false;
            bool theater = false;
            bool stadium = false;
            bool agora = false;

            const auto& rect = b.fRectTmp;
            const int xMin = rect.x;
            const int xMax = xMin + rect.w - 1;
            const int yMin = rect.y;
            const int yMax = yMin + rect.h - 1;

            for(int x = xMin - 1; x <= xMax + 1; x++) {
                for(int y = yMin - 1; y <= yMax + 1; y++) {
                    int dx;
                    int dy;
                    eTileHelper::tileIdToDTileId(x, y, dx, dy);
                    const auto aiTile = aiBoard.tile(dx, dy);
                    if(!aiTile) continue;
                    if(aiTile->fMaintanance) maintanance = true;

                    if(x >= xMin && x <= xMax &&
                       y >= yMin && y <= yMax) {
                        const auto tile = board.dtile(dx, dy);
                        if(tile) {
                            const auto terr = tile->terrain();
                            if(terr == eTerrain::fertile) {
                                result--;
                            }
                        }
                    } else if(type == eBuildingType::commonHouse ||
                              type == eBuildingType::eliteHousing) {
                        const auto ttype = aiTile->fBuilding;
                        const auto heat = eHeatGetters::appeal(ttype);
                        result += heat.fValue;
                    }
                }
            }

            if(type == eBuildingType::commonHouse ||
               type == eBuildingType::eliteHousing) {
                int minDistToUgly = 5;

                for(int x = xMin - 5; x <= xMax + 5; x++) {
                    for(int y = yMin - 5; y <= yMax + 5; y++) {
                        if(x >= xMin && x <= xMax &&
                           y >= yMin && y <= yMax) {
                            continue;
                        }
                        int dx;
                        int dy;
                        eTileHelper::tileIdToDTileId(x, y, dx, dy);
                        const auto aiTile = aiBoard.tile(dx, dy);
                        if(!aiTile) continue;
                        const auto ttype = aiTile->fBuilding;
                        const auto heat = eHeatGetters::appeal(ttype);
                        if(heat.fValue < 0) {
                            for(int xx = xMin; xx <= xMax; xx++) {
                                for(int yy = yMin; yy <= yMax; yy++) {
                                    const int dist = sqrt(pow(xx - x, 2) + pow(yy - y, 2));
                                    if(dist < minDistToUgly) minDistToUgly = dist;
                                }
                            }
                        }
                    }
                }
                result += 5*minDistToUgly;
            }

            result += 5*rect.w*rect.h;

            if(maintanance) {
                result += 5;
            }

            if(type == eBuildingType::commonHouse) {
                if(security) {
                    result += 5;
                }
                if(health) {
                    result += 5;
                }
                if(tax) {
                    result += 5;
                }
                if(podium) {
                    result += 5;
                }
                if(gymnasium) {
                    result += 5;
                }
                if(theater) {
                    result += 5;
                }
                if(stadium) {
                    result += 5;
                }
                if(agora) {
                    result += 5;
                }
            }
        }

        if(placed != 0) {
            result -= round(double(bRect.w*bRect.h)/placed);
        }

        return result;
    }

    eCityId fCid;
    eAIRoadPath fRoads;
    std::vector<eAICBuilding> fTmpBuildings;
    std::vector<eAICBuilding> fBuildings;
};

eAICityPlanningTask::eAICityPlanningTask(
        eGameBoard& board,
        const SDL_Rect& bRect,
        const ePlayerId pid,
        const eCityId cid) :
    eTask(cid), mBoard(board), mBRect(bRect), mPid(pid) {

}

struct eAICSpeciman {
    int grade(eThreadBoard& board, eAIBoard& aiBoard) const {
        int result = 0;
        for(const auto& d : fDistricts) {
            result += d.grade(board, aiBoard);
        }
        return result;
    }

    bool mutate(eThreadBoard& board) {
        if(fDistricts.empty()) return false;
        auto& d = fDistricts[eRand::rand() % fDistricts.size()];

        enum class eType {
            move,
            changeRoad,
            swapBuildings,

            count
        };

        const int itype = eRand::rand() % static_cast<int>(eType::count);
        const auto type = static_cast<eType>(itype);
        switch(type) {
        case eType::move: {
            const int byX = 2 - (eRand::rand() % 5);
            const int byY = 2 - (eRand::rand() % 5);
            return d.move(board, byX, byY);
        } break;
        case eType::changeRoad: {
            return d.changeRoad(board);
        } break;
        case eType::swapBuildings: {
            return d.swapBuildings();
        } break;
        case eType::count:
            break;
        }
        return false;
    }

    void distributeBuildings(eThreadBoard& board, eAIBoard& aiBoard) {
        for(auto& d : fDistricts) {
            d.distributeBuildings(board, aiBoard);
        }
    }

    eAICityPlan cityPlan(const ePlayerId pid,
                         const eCityId cid) {
        eAICityPlan result(pid, cid);

        for(auto& d : fDistricts) {
            d.addToCityPlan(result);
        }

        return result;
    }

    std::vector<eAICDistrict> fDistricts;
    int fGrade = 0;
};

void eAICityPlanningTask::run(eThreadBoard& data) {
    eAIBoard aiBoard;

    std::vector<eAICSpeciman> population;

    const int iterations = 100;
    const int popSize = 100;
    const int mutateSize = 25;

    enum class eDistrictType {
        commonHousing,
        eliteHousing,
        farms
    };

    std::vector<eDistrictType> dists = {eDistrictType::farms,
                                        eDistrictType::commonHousing,
                                        eDistrictType::eliteHousing};

    const auto dist = dists[mStage];

    eHeatMap map;

    if(dist == eDistrictType::commonHousing) {
        eHeatMapTask::sRun(data, &eHeatGetters::distanceFromBuilding<4, 5, 5>, map);
        {
            eHeatMap map2;
            eHeatMapTask::sRun(data, &eHeatGetters::notFertile, map2);
            map.add(map2);
        }
        {
            eHeatMap map2;
            eHeatMapTask::sRun(data, &eHeatGetters::distanceFromNotBuildable<8, 5, 5>, map2);
            map.add(map2);
        }
    } else if(dist == eDistrictType::eliteHousing) {
        eHeatMapTask::sRun(data, &eHeatGetters::distanceFromBuilding<4, 5, 5>, map);
        {
            eHeatMap map2;
            eHeatMapTask::sRun(data, &eHeatGetters::notFertile, map2);
            map.add(map2);
        }
        {
            eHeatMap map2;
            eHeatMapTask::sRun(data, &eHeatGetters::distanceFromNotBuildable<8, 5, 5>, map2);
            map.add(map2);
        }
    } else if(dist == eDistrictType::farms) {
        eHeatMapTask::sRun(data, &eHeatGetters::fertile, map);
        {
            eHeatMap map2;
            eHeatMapTask::sRun(data, &eHeatGetters::distanceFromNotBuildable<6, 5, 5>, map2);
            map.add(map2);
        }
    }

    eHeatMapDivisor divisor(map);
    divisor.divide(10);

    for(int i = 0; i < popSize; i++) {
        auto& s = population.emplace_back();
        if(dist == eDistrictType::commonHousing) {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;

            {
                int dtx;
                int dty;
                const bool r = divisor.randomHeatTile(dtx, dty);
                if(r) {
                    eTileHelper::dtileIdToTileId(dtx, dty, road.fX, road.fY);
                } else {
                    const int drx = mBRect.x + (eRand::rand() % mBRect.w);
                    const int dry = mBRect.y + (eRand::rand() % mBRect.h);
                    eTileHelper::dtileIdToTileId(drx, dry, road.fX, road.fY);
                }
            }

            district.addBuilding(eBuildingType::commonAgora);
            for(int i = 0; i < 36; i++) {
                district.addBuilding(eBuildingType::commonHouse);
            }
            district.addBuilding(eBuildingType::maintenanceOffice);
            district.addBuilding(eBuildingType::taxOffice);
            district.addBuilding(eBuildingType::gymnasium);
            district.addBuilding(eBuildingType::podium);
            district.addBuilding(eBuildingType::watchPost);
            district.addBuilding(eBuildingType::fountain);
            district.addBuilding(eBuildingType::theater);

            district.addBuilding(eBuildingType::granary);
            district.addBuilding(eBuildingType::warehouse);
        } else if(dist == eDistrictType::eliteHousing) {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;

            {
                int dtx;
                int dty;
                const bool r = divisor.randomHeatTile(dtx, dty);
                if(r) {
                    eTileHelper::dtileIdToTileId(dtx, dty, road.fX, road.fY);
                } else {
                    const int drx = mBRect.x + (eRand::rand() % mBRect.w);
                    const int dry = mBRect.y + (eRand::rand() % mBRect.h);
                    eTileHelper::dtileIdToTileId(drx, dry, road.fX, road.fY);
                }
            }

            district.addBuilding(eBuildingType::grandAgora);
            for(int i = 0; i < 12; i++) {
                district.addBuilding(eBuildingType::eliteHousing);
            }
            district.addBuilding(eBuildingType::maintenanceOffice);
            district.addBuilding(eBuildingType::taxOffice);
            district.addBuilding(eBuildingType::gymnasium);
            district.addBuilding(eBuildingType::podium);
            district.addBuilding(eBuildingType::theater);

            district.addBuilding(eBuildingType::granary);
            district.addBuilding(eBuildingType::warehouse);
        } else if(dist == eDistrictType::farms) {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;

            {
                int dtx;
                int dty;
                const bool r = divisor.randomHeatTile(dtx, dty);
                if(r) {
                    eTileHelper::dtileIdToTileId(dtx, dty, road.fX, road.fY);
                } else {
                    const int drx = mBRect.x + (eRand::rand() % mBRect.w);
                    const int dry = mBRect.y + (eRand::rand() % mBRect.h);
                    eTileHelper::dtileIdToTileId(drx, dry, road.fX, road.fY);
                }
            }

            for(int i = 0; i < 8; i++) {
                district.addBuilding(eBuildingType::wheatFarm);
            }
            district.addBuilding(eBuildingType::maintenanceOffice);
            district.addBuilding(eBuildingType::granary);
        }

        aiBoard.initialize(data.width(), data.height());
        s.distributeBuildings(data, aiBoard);
        s.fGrade = s.grade(data, aiBoard);
    }

    int lastBestGrade = 0;
    int bestGradeSince = 0;
    for(int i = 0; bestGradeSince < iterations; i++) {
        for(int j = 0; j < mutateSize; j++) {
            auto& s = population.emplace_back();
            const int srcId = eRand::rand() % popSize;
            const auto& srcS = population[srcId];
            s = srcS;
            bool c = false;
            const int kMax = 1 + (eRand::rand() % 3);
            for(int k = 0; k < kMax; k++) {
                c = c || s.mutate(data);
            }
            if(c) {
                aiBoard.initialize(data.width(), data.height());
                s.distributeBuildings(data, aiBoard);
                s.fGrade = s.grade(data, aiBoard);
            }
        }

        std::sort(population.begin(), population.end(),
                  [](const eAICSpeciman& s1, const eAICSpeciman& s2) {
            return s1.fGrade > s2.fGrade;
        });

        for(int j = 0; j < mutateSize; j++) {
            population.pop_back();
        }
        const int newBestGrade = population.front().fGrade;
        if(newBestGrade != lastBestGrade) {
            lastBestGrade = newBestGrade;
            bestGradeSince = 0;
        } else {
            bestGradeSince++;
        }
        printf("iter %d, best grade %d\n", i, newBestGrade);
    }

    const auto s = new eAICSpeciman();
    *s = population.front();
    mBest = s;
}

void eAICityPlanningTask::finish() {
    const auto s = static_cast<eAICSpeciman*>(mBest);
    auto plan = s->cityPlan(mPid, cid());
    plan.buildAllDistricts(mBoard);
    delete s;
    mBest = nullptr;
    if(++mStage > 2) return;
    auto& tp = mBoard.threadPool();
    const auto task = new eAICityPlanningTask(mBoard, mBRect, mPid, cid());
    task->mStage = mStage;
    tp.queueTask(task);
}
