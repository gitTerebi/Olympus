#ifndef EPYRAMID_H
#define EPYRAMID_H

#include "../emonument.h"

enum class eGodType;

struct ePyramidSettings {
    eBuildingType fType;
    std::vector<bool> fLevels;
};

class ePyramid : public eMonument {
public:
    ePyramid(eGameBoard& board,
             const eBuildingType type,
             const int sw, const int sh,
             const eCityId cid);

    void erase() override;

    void initialize(const std::vector<bool>& levels);

    void buildingProgressed() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    eSanctCost swapMarbleIfDark(const int e, eSanctCost cost) const;

    bool darkLevel(const int n) const { return mDark[n]; }

    static void sDimensions(const eBuildingType type,
                            int& sw, int& sh);
    static int sLevels(const eBuildingType type);
    static eGodType sGod(const eBuildingType type);
    static eBuildingType sSwitchGod(const eBuildingType srcType,
                                    const eGodType god);
    static bool sIsToGod(const eBuildingType type);
private:
    stdsptr<ePyramid> mSelf;

    std::vector<bool> mDark;
};

#endif // EPYRAMID_H
