#ifndef EEMPLOYINGBUILDING_H
#define EEMPLOYINGBUILDING_H

#include "ebuildingwithresource.h"

class eSaveArchive;

class eEmployingBuilding : public eBuildingWithResource {
public:
    eEmployingBuilding(GameBoard& board,
                       const eBuildingType type,
                       const int sw, const int sh,
                       const int maxEmployees,
                       const eCityId cid);
    ~eEmployingBuilding();

    int employed() const { return mEmployed; }
    virtual void setEmployed(const int e);
    void setMaxEmployees(const int m) { mMaxEmployees = m; }
    int maxEmployees() const { return mMaxEmployees; }
    double vacanciesFilledFraction() const;
    double effectiveness() const;
    bool shutDown() const { return mShutDown; }
    void setShutDown(const bool sd);

protected:
    void serializeFields(eSaveArchive& ar) override;

private:
    bool mShutDown = false;
    int mMaxEmployees;
    int mEmployed = 0;
};

#endif // EEMPLOYINGBUILDING_H
