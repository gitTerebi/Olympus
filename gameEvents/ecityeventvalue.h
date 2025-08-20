#ifndef ECITYEVENTVALUE_H
#define ECITYEVENTVALUE_H

#include "engine/eworldcity.h"

class eCityEventValue {
public:
    eCityEventValue(eGameBoard& board);

    void write(eWriteStream& dst) const;
    void read(eReadStream& src, eGameBoard& board);

    void setSingleCity(const stdsptr<eWorldCity>& c);
    const stdsptr<eWorldCity>& city() const { return mCity; }

    int minCityId() const { return mMinCityId; }
    void setMinCityId(const int min) { mMinCityId = min; }

    int maxCityId() const { return mMaxCityId; }
    void setMaxCityId(const int max) { mMaxCityId = max; }
protected:
    void chooseCity();

    void longNameReplaceCity(const std::string& id,
                             std::string& tmpl) const;

    stdsptr<eWorldCity> mCity;
    int mMinCityId = 0;
    int mMaxCityId = 0;
private:
    int chooseCityId() const;

    eGameBoard& mBoard;
};

#endif // ECITYEVENTVALUE_H
