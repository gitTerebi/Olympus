#ifndef AICityPlan_H
#define AICityPlan_H

#include <SDL2/SDL_rect.h>
#include <vector>

#include "pointers/estdselfref.h"
#include "engine/eorientation.h"

#include "ai-district.h"

enum class eBuildingType;
enum class eResourceType;
enum class ePlayerId;
enum class eCityId;
class GameBoard;
class eSaveArchive;

class AICityPlan {
public:
    AICityPlan(const eCityId cid);

    void addScheduledBuilding(const int did, const SDL_Rect& bRect);
    void addScheduledBuilding(const int did, const AIBuilding& b);

    int districtCount() const;
    AIDistrict& district(const int id);
    void addDistrict(const AIDistrict& a);

    int districtCost(GameBoard& board, const int id,
                     int* const marble = nullptr) const;
    int nextDistrictId() const;
    int lastBuiltDistrictId() const;

    bool buildNextDistrict(GameBoard& board);
    void buildAllDistricts(GameBoard& board);
    void buildScheduled(GameBoard& board);
    bool districtBuilt(const int id) const;

    void editorDisplayBuildings(GameBoard& board);

    void serialize(eSaveArchive& ar);
private:

    eCityId mCid;

    int mLastBuildDistrict = -1;
    std::vector<AIDistrict> mDistricts;
    std::vector<std::pair<int, AIBuilding>> mScheduledBuildings;
};

#endif // AICityPlan_H
