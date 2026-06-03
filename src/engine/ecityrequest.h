#ifndef ECITYREQUEST_H
#define ECITYREQUEST_H

#include "world-city.h"
#include "eresourcetype.h"

struct eCityRequest {
    bool operator==(const eCityRequest& o) const {
        return fCity == o.fCity &&
               fType == o.fType &&
               fCount == o.fCount;
    }

    stdsptr<WorldCity> fCity;
    eResourceType fType = eResourceType::none;
    int fCount = 0;
};

#endif // ECITYREQUEST_H
