#ifndef ECITYEVENTVALUE_H
#define ECITYEVENTVALUE_H

#include "engine/world-city.h"

class eSaveArchive;

class eCityEventValue {
public:
    using eValidator = std::function<bool(WorldCity&)>;
    eCityEventValue(GameBoard& board,
                    const eValidator& v = nullptr);

    void setSingleCity(const stdsptr<WorldCity>& c);
    const stdsptr<WorldCity>& city() const { return mCity; }

    int minCityId() const { return mMinCityId; }
    void setMinCityId(const int min) { mMinCityId = min; }

    int maxCityId() const { return mMaxCityId; }
    void setMaxCityId(const int max) { mMaxCityId = max; }
protected:
    void serialize(eSaveArchive& ar, GameBoard& board);

    void chooseCity();

    void longNameReplaceCity(const std::string& id,
                             std::string& tmpl) const;

    stdsptr<WorldCity> mCity;
    int mMinCityId = 0;
    int mMaxCityId = 0;
private:
    int chooseCityId() const;

    GameBoard& mBoard;
    const eValidator mValidator;
};

#endif // ECITYEVENTVALUE_H
