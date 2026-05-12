#ifndef ERESOURCECOLLECTBUILDINGBASE_H
#define ERESOURCECOLLECTBUILDINGBASE_H

#include "eresourcebuildingbase.h"
#include <array>

class eResourceCollectBuildingBase : public eResourceBuildingBase {
public:
    using eResourceBuildingBase::eResourceBuildingBase;

    virtual void addRaw() = 0;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(class eJsonArchive& ar) override;

    void nextMonth() override;

    int producedThisYear() const { return mProducedThisYear; }

    void setNoTarget(const bool t);
    bool noTarget() const { return mNoTarget; }
protected:
    void trackProduced(const int c);

private:
    bool mNoTarget = true;

    int mProducedThisYear = 0;
    std::array<int,12> mMonthlyProduced{};
    int mRingIdx = 0;
};

#endif // ERESOURCECOLLECTBUILDINGBASE_H
