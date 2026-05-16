#ifndef ECITYREQUEST_H
#define ECITYREQUEST_H

#include "e-worldcity.h"
#include "eresourcetype.h"
#include "fileIO/esavearchive.h"

struct eCityRequest {
    void serialize(eSaveArchive& ar, eGameBoard* board) {
        ar.payloadField("city",
            [this](eWriteStream& dst) { dst.writeCity(fCity.get()); },
            [this, board](eReadStream& src) {
                src.readCity(board, [this](const stdsptr<eWorldCity>& city) {
                    fCity = city;
                });
            });
        ar.field("resource", fType);
        ar.field("count", fCount);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        const_cast<eCityRequest*>(this)->serialize(ar, nullptr);
    }

    void read(eGameBoard& board, eReadStream& src) {
        eSaveArchive ar(src);
        serialize(ar, &board);
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
