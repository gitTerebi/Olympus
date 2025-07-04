#ifndef EAICITYPLAN_H
#define EAICITYPLAN_H

#include <SDL2/SDL_rect.h>
#include <vector>

#include "pointers/estdselfref.h"
#include "engine/eorientation.h"

#include "eaidistrict.h"

enum class eBuildingType;
enum class eResourceType;
enum class ePlayerId;
enum class eCityId;
class eGameBoard;

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

class eAICityPlan {
public:
    eAICityPlan(const eCityId cid);

    eAIBoard aiBoard(const int w, const int h) const;

    int districtCount() const;
    eAIDistrict& district(const int id);
    void addDistrict(const eAIDistrict& a);

    int districtCost(eGameBoard& board, const int id) const;
    int nextDistrictId();

    void buildDistrict(eGameBoard& board, const int id);
    void buildAllDistricts(eGameBoard& board);
    void rebuildDistricts(eGameBoard& board);
    using ePoints = std::vector<std::pair<SDL_Point, SDL_Point>>;
    bool connectDistricts(const int id1, const int id2,
                          ePoints& points);
    void connectAllBuiltDistricts(eGameBoard& board);
    bool districtBuilt(const int id) const;

    void editorDisplayBuildings(eGameBoard& board);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    eCityId mCid;

    std::vector<int> mBuiltDistrics;
    std::vector<eAIDistrict> mDistricts;
};

#endif // EAICITYPLAN_H
