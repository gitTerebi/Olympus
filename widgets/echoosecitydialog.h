#ifndef ECHOOSECITYDIALOG_H
#define ECHOOSECITYDIALOG_H

#include "echoosebutton.h"

#include "engine/eworldcity.h"

class eChooseCityDialog : public eChooseButton {
public:
    using eChooseButton::eChooseButton;

    using eCityAction = std::function<void(const stdsptr<eWorldCity>&)>;
    void initialize(eWorldBoard* const board, const eCityAction& cact);

    using eCityValidator = std::function<bool(const stdsptr<eWorldCity>&)>;
    void setValidator(const eCityValidator& v);
private:
    eCityValidator mValidator;
};

#endif // ECHOOSECITYDIALOG_H
