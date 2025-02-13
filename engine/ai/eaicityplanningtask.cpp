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
        case eOrientation::topRight:
            return fX;
        case eOrientation::bottomRight:
            return fX + fLen - 1;
        case eOrientation::bottomLeft:
            return fX;
        case eOrientation::topLeft:
            return fX - fLen + 1;
        default:
            return fX;
        }
    }

    int yEnd() const {
        switch(fO) {
        case eOrientation::topRight:
            return fY - fLen + 1;
        case eOrientation::bottomRight:
            return fY;
        case eOrientation::bottomLeft:
            return fY + fLen - 1;
        case eOrientation::topLeft:
            return fY;
        default:
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
        case eOrientation::topRight:
            x = fX;
            y = fY - displ;
            break;
        case eOrientation::bottomRight:
            x = fX + displ;
            y = fY;
            break;
        case eOrientation::bottomLeft:
            x = fX;
            y = fY + displ;
            break;
        case eOrientation::topLeft:
            x = fX - displ;
            y = fY;
            break;
        default:
            break;
        }
    }

    void updateCoordinates() {
        for(auto& b : fBranches) {
            coordinatesAt(b.fDisplacement, b.fX, b.fY);
            b.updateCoordinates();
        }
    }

    void allBranches(std::vector<eAIRoadPath*>& result) {
        result.push_back(this);
        for(auto& b : fBranches) {
            b.allBranches(result);
        }
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

    int fX;
    int fY;
    int fDisplacement;
    eOrientation fO;
    int fLen;

    std::vector<eAIRoadPath> fBranches;
};

bool gHasRoad(const int xMin, const int yMin,
              const int xMax, const int yMax,
              eRoadBoard& roadBoard) {
    for(int x = xMin; x <= xMax; x++) {
        for(int y = yMin; y <= yMax; y++) {
            int dx;
            int dy;
            eTileHelper::tileIdToDTileId(x, y, dx, dy);
            const auto tile = roadBoard.tile(dx, dy);
            if(!tile) continue;
            const bool r = *tile == 1;
            if(r) return true;
        }
    }
    return false;
}

bool gNextToRoad(const int xMin, const int yMin,
                 const int xMax, const int yMax,
                 eRoadBoard& roadBoard,
                 std::vector<eDiagonalOrientation>* const o) {
    const bool r1 = gHasRoad(xMin, yMin - 1, xMax, yMin - 1, roadBoard);
    if(r1) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::topRight);
    }
    const bool r2 = gHasRoad(xMin, yMax + 1, xMax, yMax + 1, roadBoard);
    if(r2) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::bottomLeft);
    }
    const bool r3 = gHasRoad(xMin - 1, yMin, xMin - 1, yMax, roadBoard);
    if(r3) {
        if(!o) return true;
        o->push_back(eDiagonalOrientation::topLeft);
    }
    const bool r4 = gHasRoad(xMax + 1, yMin, xMax + 1, yMax, roadBoard);
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
        fRoads.allBranches(allRoads);
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

        result.addDistrict(district);
    }

    bool validRoad(eThreadBoard& board,
                   const int xMin, const int xMax,
                   const int yMin, const int yMax) {
        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                const auto tile = board.tile(x, y);
                if(!tile) return false;
                const auto terr = tile->terrain();
                const bool b = static_cast<bool>(terr & eTerrain::buildableAfterClear);
                if(!b) return false;
                const auto cid = tile->cityId();
                if(cid != fCid) return false;
            }
        }
        return true;
    }

    bool validRoad(eThreadBoard& board,
                   const eAIRoadPath& road) {
        const int xMin = road.minX();
        const int xMax = road.maxX();
        const int yMin = road.minY();
        const int yMax = road.maxY();

        return validRoad(board, xMin, xMax, yMin, yMax);
    }

    bool changeRoad(eThreadBoard& board) {
        enum class eType {
            add,
            remove,
            resize,
            move,

            count
        };

        const int itype = eRand::rand() % static_cast<int>(eType::count);
        auto type = static_cast<eType>(itype);
        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads);
        const int nR = allRoads.size();
        if(nR == 1 && type == eType::remove) return false;
        const int srcRoadId = eRand::rand() % allRoads.size();
        auto& srcRoad = *allRoads[srcRoadId];
        switch(type) {
        case eType::add: {
            eAIRoadPath newRoad;
            newRoad.fDisplacement = 2 + (eRand::rand() % 10);
            newRoad.fLen = 2 + (eRand::rand() % 10);
            srcRoad.coordinatesAt(newRoad.fDisplacement, newRoad.fX, newRoad.fY);
            const int oi = eRand::rand() % 2;
            switch(srcRoad.fO) {
            case eOrientation::topRight:
            case eOrientation::bottomLeft: {
                if(oi) {
                    newRoad.fO = eOrientation::topLeft;
                } else {
                    newRoad.fO = eOrientation::bottomRight;
                }
            } break;
            case eOrientation::topLeft:
            case eOrientation::bottomRight:
            default: {
                if(oi) {
                    newRoad.fO = eOrientation::topRight;
                } else {
                    newRoad.fO = eOrientation::bottomLeft;
                }
            } break;
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
            tmp.updateCoordinates();
            std::vector<eAIRoadPath*> allTmp;
            tmp.allBranches(allTmp);
            for(const auto t : allTmp) {
                const bool r = validRoad(board, *t);
                if(!r) return false;
            }
            srcRoad = tmp;
            return true;
        } break;
        case eType::move: {
            auto& bs = srcRoad.fBranches;
            if(bs.empty()) return false;
            const int toMoveId = eRand::rand() % bs.size();
            auto& toMove = bs[toMoveId];
            eAIRoadPath tmp = toMove;
            tmp.fDisplacement += 2 - (eRand::rand() % 5);
            srcRoad.coordinatesAt(tmp.fDisplacement, tmp.fX, tmp.fY);
            tmp.updateCoordinates();
            std::vector<eAIRoadPath*> allTmp;
            tmp.allBranches(allTmp);
            for(const auto t : allTmp) {
                const bool r = validRoad(board, *t);
                if(!r) return false;
            }
            toMove = tmp;
            return true;
        } break;
        case eType::count:
            return false;
        }

        return false;
    }

    bool move(eThreadBoard& board,
              const int byX, const int byY) {
        std::vector<eAIRoadPath*> allRoads;
        fRoads.allBranches(allRoads);
        for(const auto road : allRoads) {
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
                       eAICBuilding& b) {
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
                if(isAgora) {

                } else {
                    std::vector<eDiagonalOrientation> os;
                    const bool nextToRoad = gNextToRoad(xMin, yMin, xMax, yMax, roadBoard,
                                                        useAvenue ? &os : nullptr);
                    if(!nextToRoad) continue;
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
                        const auto terr = vtile->terrain();
                        const bool f = static_cast<bool>(terr & eTerrain::fertile);
                        if(f) foundFertile = true;
                        const bool v = static_cast<bool>(terr & eTerrain::buildableAfterClear);
                        if(!v) {
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
        fRoads.allBranches(allRoads);
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
                    const auto tile = aiBoard.tile(dx, dy);
                    if(!tile) continue;
                    if(tile->fBuilding != eBuildingType::none) continue;
                    const auto btile = board.dtile(dx, dy);
                    if(!btile) continue;
                    if(btile->cityId() != fCid) continue;
                    tile->fBuilding = eBuildingType::road;
                    {
                        const auto tile = roadBoard.tile(dx, dy);
                        *tile = 1;
                    }
                }
            }
        }

        for(auto& b : fBuildings) {
            b.fRectTmp = {0, 0, 0, 0};
            placeBuilding(roadsBRect, board, aiBoard, roadBoard, b);
        }
    }

    int grade(eThreadBoard& board,
              eAIBoard& aiBoard) const {
        int result = 0;

        result -= fRoads.totalBranchesCount();
        result -= fRoads.totalBranchesLen();

        result -= fBuildings.size();

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
                    }
                }
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

    const int iterations = 1000;
    const int popSize = 100;
    const int mutateSize = 25;

    for(int i = 0; i < popSize; i++) {
        auto& s = population.emplace_back();
        {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;
            const int drx = mBRect.x + (eRand::rand() % mBRect.w);
            const int dry = mBRect.y + (eRand::rand() % mBRect.h);
            eTileHelper::dtileIdToTileId(drx, dry, road.fX, road.fY);
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
        }
        {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;
            const int drx = mBRect.x + (eRand::rand() % mBRect.w);
            const int dry = mBRect.y + (eRand::rand() % mBRect.h);
            eTileHelper::dtileIdToTileId(drx, dry, road.fX, road.fY);
            district.addBuilding(eBuildingType::grandAgora);
            for(int i = 0; i < 12; i++) {
                district.addBuilding(eBuildingType::eliteHousing);
            }
            district.addBuilding(eBuildingType::maintenanceOffice);
            district.addBuilding(eBuildingType::taxOffice);
            district.addBuilding(eBuildingType::gymnasium);
            district.addBuilding(eBuildingType::podium);
            district.addBuilding(eBuildingType::theater);
        }
        {
            auto& district = s.fDistricts.emplace_back();
            district.fCid = cid();
            auto& road = district.fRoads;
            road.fLen = 4;

            {
                eHeatMap map;
                eHeatMapTask::sRun(data, &eHeatGetters::fertile, map);

                eHeatMapDivisor divisor(map);
                divisor.divide(10);
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

    for(int i = 0; i < iterations; i++) {
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
        printf("iter %d, best grade %d\n", i, population.front().fGrade);
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
}
