#ifndef EPYRAMID_H
#define EPYRAMID_H

#include "../emonument.h"

class ePyramid : public eMonument {
public:
    ePyramid(eGameBoard& board,
             const eBuildingType type,
             const int sw, const int sh,
             const eCityId cid);

    void erase() override;

    void initialize();

    void buildingProgressed() override;

    void read(eReadStream& src) override;

    eSanctCost swapMarbleIfDark(const int e, eSanctCost cost) const;

    bool darkLevel(const int n) const { return mDark[n]; }

    static void sDimensions(const eBuildingType type,
                            int& sw, int& sh);
private:
    stdsptr<ePyramid> mSelf;

    std::vector<bool> mDark;
};

#endif // EPYRAMID_H
