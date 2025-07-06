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

class eAICityPlan {
public:
    eAICityPlan(const eCityId cid);

    void addScheduledBuilding(const int did, const SDL_Rect& bRect);
    void addScheduledBuilding(const int did, const eAIBuilding& b);

    int districtCount() const;
    eAIDistrict& district(const int id);
    void addDistrict(const eAIDistrict& a);

    int districtCost(eGameBoard& board, const int id) const;
    int nextDistrictId() const;
    int lastBuiltDistrictId() const;

    bool buildNextDistrict(eGameBoard& board);
    void buildAllDistricts(eGameBoard& board);
    void buildScheduled(eGameBoard& board);
    bool districtBuilt(const int id) const;

    void editorDisplayBuildings(eGameBoard& board);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    eCityId mCid;

    int mLastBuildDistrict = -1;
    std::vector<eAIDistrict> mDistricts;
    std::vector<std::pair<int, eAIBuilding>> mScheduledBuildings;
};

#endif // EAICITYPLAN_H
