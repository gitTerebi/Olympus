#ifndef AI_DISTRICT_H
#define AI_DISTRICT_H

#include "ai-building.h"

#include "engine/eresourcetype.h"
#include "characters/gods/god.h"

class SaveArchive;

struct eDistrictReadyCondition {
    enum class eType {
        districtResourceCount,
        totalResourceCount,
        districtPopulation,
        totalPopulation,
        sanctuaryReady,
        count
    };

    static std::string sName(const eType type);

    std::string name() const;

    void serialize(SaveArchive& ar);
    eType fType;
    eResourceType fResource = eResourceType::drachmas;
    GodType fSanctuary = GodType::zeus;
    int fValue = 0;
};

class AIDistrict {
public:
    using eScheduleBuildingAction = std::function<void(const AIBuilding&)>;
    void build(GameBoard& board,
               const ePlayerId pid,
               const eCityId cid,
               const bool editorDisplay,
               const eScheduleBuildingAction& scha = nullptr) const;
    using eAction = std::function<void()>;
    static bool sBuild(GameBoard& board,
                       const ePlayerId pid,
                       const eCityId cid,
                       const bool editorDisplay,
                       const AIBuilding& b);

    bool road(int& x, int& y) const;

    void addBuilding(const AIBuilding& a);

    void serialize(SaveArchive& ar);

    std::vector<AIBuilding> fBuildings;
    std::vector<eDistrictReadyCondition> fReadyConditions;
};

#endif // AI_DISTRICT_H
