#ifndef ECITYBUTTON_H
#define ECITYBUTTON_H

#include "framed-button.h"

#include "engine/world-city.h"

class GameBoard;

class eCityButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eCityAction = std::function<void(const stdsptr<WorldCity>&)>;
    void initialize(WorldBoard* const board,
                    const eCityAction& cact,
                    const bool showId = false);

    using eCityValidator = std::function<bool(const stdsptr<WorldCity>&)>;
    void setValidator(const eCityValidator& v);

    const stdsptr<WorldCity>& city() const { return mCity; }
    void setCity(const stdsptr<WorldCity>& c);
private:
    bool mShowId = false;
    stdsptr<WorldCity> mCity;
    eCityValidator mValidator;
};

#endif // ECITYBUTTON_H
