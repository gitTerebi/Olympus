#ifndef ESANCTBUILDING_H
#define ESANCTBUILDING_H

#include "../ebuilding.h"

struct eSanctCost {
    int fWood;
    int fMarble;
    int fSculpture;
    int fOrichalc = 0;

    eSanctCost& operator+=(const eSanctCost& c) {
        fWood += c.fWood;
        fMarble += c.fMarble;
        fSculpture += c.fSculpture;
        fOrichalc += c.fOrichalc;
        return *this;
    }

    eSanctCost& operator-=(const eSanctCost& c) {
        fWood -= c.fWood;
        fMarble -= c.fMarble;
        fSculpture -= c.fSculpture;
        fOrichalc -= c.fOrichalc;
        return *this;
    }

    eSanctCost operator-(const eSanctCost& c1) const {
        eSanctCost c;
        c = *this;
        c.operator-=(c1);
        return c;
    }

    void read(eReadStream& src) {
        src >> fWood;
        src >> fMarble;
        src >> fSculpture;
        src >> fOrichalc;
    }

    void write(eWriteStream& dst) const {
        dst << fWood;
        dst << fMarble;
        dst << fSculpture;
        dst << fOrichalc;
    }
};

class eMonument;

class eSanctBuilding : public eBuilding {
public:
    eSanctBuilding(const std::vector<eSanctCost>& cost,
                   eGameBoard& board,
                   const eBuildingType type,
                   const int sw, const int sh,
                   const eCityId cid);

    void erase() override;
    virtual void progressed() {}

    bool resourcesAvailable() const;

    bool finished() const;
    int progress() const { return mProgress; }
    int maxProgress() const { return mMaxProgress; }
    bool incProgress();
    void destroy();

    const eSanctCost& cost() const { return mTotalCost; }

    bool workedOn() const { return mWorkedOn; }
    void setWorkedOn(const bool w) { mWorkedOn = w; }

    bool halted() const { return mHalted; }
    void setHalted(const bool h) { mHalted = h; }

    void setMonument(eMonument* const s);
    eMonument* monument() const { return mMonument; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void scheduleTerrainUpdate();
    void updateNextCost();

    const int mMaxProgress;
    const std::vector<eSanctCost> mCost{{0, 0, 0}};
    const eSanctCost mTotalCost{0, 0, 0};
    eSanctCost mNextCost{0, 0, 0};
    eMonument* mMonument = nullptr;
    bool mWorkedOn = false;
    int mProgress = 0;
    bool mHalted = false;
};

#endif // ESANCTBUILDING_H
