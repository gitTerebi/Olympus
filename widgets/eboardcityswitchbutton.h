#ifndef EBOARDCITYSWITCHBUTTON_H
#define EBOARDCITYSWITCHBUTTON_H

#include "eswitchbutton.h"

class GameBoard;

enum class eCityId;
enum class ePlayerId;

class eBoardCitySwitchButton : public eSwitchButton {
public:
    using eSwitchButton::eSwitchButton;

    using eValidator = std::function<bool(eCityId)>;
    using eSwitchAction = std::function<void(eCityId)>;
    void initialize(const GameBoard& board, const eValidator& v,
                    const eSwitchAction& a);
    void initialize(const std::map<eCityId, std::string>& map,
                    const eSwitchAction& a);

    eCityId currentCity() const;
    bool setCurrentCity(const eCityId cid);

    static eValidator sPlayerCityValidator(
            const GameBoard& board, const ePlayerId pid);
    static eValidator sPlayerEnemyCityValidator(
            const GameBoard& board, const ePlayerId pid);
private:
    using eSwitchButton::addValue;
    using eSwitchButton::currentValue;
    using eSwitchButton::setValue;
    using eSwitchButton::setSwitchAction;
    using eSwitchButton::fitValidContent;

    std::vector<eCityId> mCities;
};

#endif // EBOARDCITYSWITCHBUTTON_H
