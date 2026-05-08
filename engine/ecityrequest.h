#ifndef ECITYREQUEST_H
#define ECITYREQUEST_H

#include "eworldcity.h"
#include "eresourcetype.h"
#include "fileIO/esavearchive.h"

struct eCityRequest {
    void write(eWriteStream& dst) const {
        dst.writeCity(fCity.get());
        eSaveArchive ar(dst);
        ar.field("type", const_cast<eResourceType&>(fType));
        ar.field("count", const_cast<int&>(fCount));
    }

    void read(eGameBoard& board, eReadStream& src) {
        src.readCity(&board, [this](const stdsptr<eWorldCity>& city) {
            fCity = city;
        });
        eSaveArchive ar(src);
        ar.field("type", fType);
        ar.field("count", fCount);
    }

    bool operator==(const eCityRequest& o) const {
        return fCity == o.fCity &&
               fType == o.fType &&
               fCount == o.fCount;
    }

    stdsptr<eWorldCity> fCity;
    eResourceType fType;
    int fCount;
};

#endif // ECITYREQUEST_H
