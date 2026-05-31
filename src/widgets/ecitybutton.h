#ifndef ECITYBUTTON_H
#define ECITYBUTTON_H

#include "framed-button.h"

#include "engine/e-worldcity.h"

class GameBoard;

class eCityButton : public FramedButton {
public:
    using FramedButton::FramedButton;

    using eCityAction = std::function<void(const stdsptr<eWorldCity>&)>;
    void initialize(eWorldBoard* const board,
                    const eCityAction& cact,
                    const bool showId = false);

    using eCityValidator = std::function<bool(const stdsptr<eWorldCity>&)>;
    void setValidator(const eCityValidator& v);

    const stdsptr<eWorldCity>& city() const { return mCity; }
    void setCity(const stdsptr<eWorldCity>& c);
private:
    bool mShowId = false;
    stdsptr<eWorldCity> mCity;
    eCityValidator mValidator;
};

#endif // ECITYBUTTON_H
