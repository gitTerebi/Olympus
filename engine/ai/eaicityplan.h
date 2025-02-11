#ifndef EAICITYPLAN_H
#define EAICITYPLAN_H

#include <SDL2/SDL_rect.h>
#include <vector>

#include "pointers/estdselfref.h"
#include "engine/epatrolguide.h"
#include "engine/eorientation.h"

enum class eBuildingType;
enum class eResourceType;
enum class ePlayerId;
enum class eCityId;
class eGameBoard;

template <class T>
class eRectMap {
public:
    struct eRectT {
        SDL_Rect fR;
        stdsptr<T> fT;
    };

    eRectMap() {}

    eRectMap(const int w, const int h) {
        mW = w;
        mH = h;
        for(int x = 0; x < w; x++) {
            auto& row = mPs.emplace_back();
            for(int y = 0; y < h; y++) {
                row.emplace_back();
            }
        }
    }

    bool isEmpty(const SDL_Rect& rect) const {
        const int xMin = rect.x;
        const int yMin = rect.y;
        const int xMax = rect.x + rect.w - 1;
        const int yMax = rect.y + rect.h - 1;
        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                const auto tsptr = mPs[x][y].fT;
                if(tsptr) return false;
            }
        }
        return true;
    }

    eRectT at(const int x, const int y) {
        const SDL_Point pt{x, y};
        return at(pt);
    }

    eRectT at(const SDL_Point& pt) {
        const auto ir = inRange(pt);
        if(!ir) return nullptr;
        return {SDL_Rect(), mPs[pt.x][pt.y]};
    }

    bool add(const eRectT& rt) {
        return add(rt.fR, rt.fT);
    }

    bool add(const SDL_Rect& rect, const stdsptr<T>& t) {
        const auto ir = inRange(rect);
        if(!ir) return false;
        const bool e = isEmpty(rect);
        if(!e) return false;
        const int xMin = rect.x;
        const int yMin = rect.y;
        const int xMax = rect.x + rect.w - 1;
        const int yMax = rect.y + rect.h - 1;
        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                auto& cell = mPs[x][y];
                cell.fR = rect;
                cell.fT = t;
            }
        }
        return true;
    }

    void clear(const SDL_Rect& rect) {
        const int xMin = rect.x;
        const int yMin = rect.y;
        const int xMax = rect.x + rect.w - 1;
        const int yMax = rect.y + rect.h - 1;
        for(int x = xMin; x <= xMax; x++) {
            for(int y = yMin; y <= yMax; y++) {
                auto& cell = mPs[x][y];
                if(!cell.fT) continue;
                const auto& r = cell.fR;
                const int xMin2 = r.x;
                const int yMin2 = r.y;
                const int xMax2 = r.x + r.w - 1;
                const int yMax2 = r.y + r.h - 1;
                for(int x2 = xMin2; x2 <= xMax2; x2++) {
                    for(int y2 = yMin2; y2 <= yMax2; y2++) {
                        auto& cell = mPs[x2][y2];
                        cell.fR = SDL_Rect{0, 0, 0, 0};
                        cell.fT = nullptr;
                    }
                }
            }
        }
    }

    bool inRange(const int x, const int y) {
        const SDL_Point pt{x, y};
        return inRange(pt);
    }

    bool inRange(const SDL_Point& pt) {
        return inRange(SDL_Rect{pt.x, pt.y, 1, 1});
    }

    bool inRange(const SDL_Rect& r) {
        const int xMin = r.x;
        if(xMin < 0) return false;
        const int yMin = r.y;
        if(yMin < 0) return false;
        const int xMax = r.x + r.w - 1;
        if(xMax > mW - 1) return false;
        const int yMax = r.y + r.h - 1;
        if(yMax > mH - 1) return false;
        return true;
    }
private:
    int mW = 0;
    int mH = 0;
    std::vector<std::vector<eRectT>> mPs;
};

struct eAIBuilding {
    eBuildingType fType;
    SDL_Rect fRect;
    eResourceType fGet = static_cast<eResourceType>(0);
    eResourceType fEmpty = static_cast<eResourceType>(0);
    eResourceType fAccept = static_cast<eResourceType>(0);
    std::vector<ePatrolGuide> fGuides;
    eOrientation fO;
};

class eAIDistrict {
public:
    void build(eGameBoard& board,
               const ePlayerId pid,
               const eCityId cid) const;

    bool road(int& x, int& y) const;
private:
    std::vector<eAIBuilding> mBuildings;
};

class eAICityPlan {
public:
    eAICityPlan(const ePlayerId pid,
                const eCityId cid);

    void buildDistrict(eGameBoard& board,
                       const int id);
    void rebuildDistricts(eGameBoard& board);
    bool connectDistricts(eGameBoard& board,
                          const int id1, const int id2);
    void connectAllBuiltDistricts(eGameBoard& board);
    bool districtBuilt(const int id) const;
private:
    ePlayerId mPid;
    eCityId mCid;

    std::vector<int> mBuiltDistrics;
    std::vector<eAIDistrict> mDistricts;
};

#endif // EAICITYPLAN_H
