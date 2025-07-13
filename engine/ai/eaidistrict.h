#ifndef EAIDISTRICT_H
#define EAIDISTRICT_H

#include "eaibuilding.h"

#include "engine/eresourcetype.h"
#include "characters/gods/egod.h"

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

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    eType fType;
    eResourceType fResource = eResourceType::drachmas;
    eGodType fSanctuary = eGodType::zeus;
    int fValue = 0;
};

class eAIDistrict {
public:
    using eScheduleBuildingAction = std::function<void(const eAIBuilding&)>;
    void build(eGameBoard& board,
               const ePlayerId pid,
               const eCityId cid,
               const bool editorDisplay,
               const eScheduleBuildingAction& scha = nullptr) const;
    using eAction = std::function<void()>;
    static bool sBuild(eGameBoard& board,
                       const ePlayerId pid,
                       const eCityId cid,
                       const bool editorDisplay,
                       const eAIBuilding& b);

    bool road(int& x, int& y) const;

    void addBuilding(const eAIBuilding& a);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    std::vector<eAIBuilding> fBuildings;
    std::vector<eDistrictReadyCondition> fReadyConditions;
};

#endif // EAIDISTRICT_H
