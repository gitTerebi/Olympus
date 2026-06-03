#ifndef ECHOOSECITYDIALOG_H
#define ECHOOSECITYDIALOG_H

#include "echoosebutton.h"

#include "engine/world-city.h"

class eChooseCityDialog : public eChooseButton {
public:
    using eChooseButton::eChooseButton;

    using eCityAction = std::function<void(const stdsptr<WorldCity>&)>;
    void initialize(WorldBoard* const board,
                    const eCityAction& cact,
                    const bool showId = false);

    using eCityValidator = std::function<bool(const stdsptr<WorldCity>&)>;
    void setValidator(const eCityValidator& v);
private:
    eCityValidator mValidator;
};

#endif // ECHOOSECITYDIALOG_H
