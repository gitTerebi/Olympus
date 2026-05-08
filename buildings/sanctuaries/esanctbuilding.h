#ifndef ESANCTBUILDING_H
#define ESANCTBUILDING_H

#include "../ebuilding.h"
#include "fileIO/esavearchive.h"

struct eSanctCost {
    int fWood;
    int fMarble;
    int fSculpture;
    int fOrichalc = 0;
    int fBlackMarble = 0;

    void switchMarble() {
        std::swap(fMarble, fBlackMarble);
    }

    eSanctCost& operator+=(const eSanctCost& c) {
        fWood += c.fWood;
        fMarble += c.fMarble;
        fSculpture += c.fSculpture;
        fOrichalc += c.fOrichalc;
        fBlackMarble += c.fBlackMarble;
        return *this;
    }

    eSanctCost& operator-=(const eSanctCost& c) {
        fWood -= c.fWood;
        fMarble -= c.fMarble;
        fSculpture -= c.fSculpture;
        fOrichalc -= c.fOrichalc;
        fBlackMarble -= c.fBlackMarble;
        return *this;
    }

    eSanctCost operator-(const eSanctCost& c1) const {
        eSanctCost c;
        c = *this;
        c.operator-=(c1);
        return c;
    }

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        ar.field("wood", fWood);
        ar.field("marble", fMarble);
        ar.field("sculpture", fSculpture);
        ar.field("orichalc", fOrichalc);
        ar.field("blackMarble", fBlackMarble);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("wood", const_cast<int&>(fWood));
        ar.field("marble", const_cast<int&>(fMarble));
        ar.field("sculpture", const_cast<int&>(fSculpture));
        ar.field("orichalc", const_cast<int&>(fOrichalc));
        ar.field("blackMarble", const_cast<int&>(fBlackMarble));
    }
};

class eMonument;
class eSaveArchive;

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

    const std::vector<eSanctCost>& costs() const { return mCost; }
private:
    void serialize(eSaveArchive& ar);

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
