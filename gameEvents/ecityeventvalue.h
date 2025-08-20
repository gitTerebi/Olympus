#ifndef ECITYEVENTVALUE_H
#define ECITYEVENTVALUE_H

#include "engine/eworldcity.h"

class eCityEventValue {
public:
    void write(eWriteStream& dst) const;
    void read(eReadStream& src, eGameBoard& board);

    void setCity(const stdsptr<eWorldCity>& c) { mCity = c; }
    const stdsptr<eWorldCity>& city() const { return mCity; }
protected:
    void longNameReplaceCity(const std::string& id,
                             std::string& tmpl) const;

    stdsptr<eWorldCity> mCity;
};

#endif // ECITYEVENTVALUE_H
