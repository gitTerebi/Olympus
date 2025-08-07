#ifndef EPYRAMID_H
#define EPYRAMID_H

#include "../emonument.h"

enum class eGodType;

class ePyramid : public eMonument {
public:
    ePyramid(eGameBoard& board,
             const eBuildingType type,
             const int sw, const int sh,
             const eCityId cid);

    void erase() override;

    void initialize(const eGodType god);

    void buildingProgressed() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    eSanctCost swapMarbleIfDark(const int e, eSanctCost cost) const;

    bool darkLevel(const int n) const { return mDark[n]; }

    eGodType god() const { return mGod; }

    static void sDimensions(const eBuildingType type,
                            int& sw, int& sh);
private:
    stdsptr<ePyramid> mSelf;

    eGodType mGod;
    std::vector<bool> mDark;
};

#endif // EPYRAMID_H
