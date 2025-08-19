#include "ecitybutton.h"

#include "echoosecitydialog.h"
#include "emainwindow.h"
#include "engine/egameboard.h"
#include "elanguage.h"

void eCityButton::initialize(eWorldBoard* const board,
                             const eCityAction& cact,
                             const bool showId) {
    setUnderline(false);
    setPressAction([this, board, cact, showId]() {
        const auto choose = new eChooseCityDialog(window());
        choose->setValidator(mValidator);
        const auto act = [this, cact](const stdsptr<eWorldCity>& c) {
            setCity(c);
            if(cact) cact(c);
        };
        choose->initialize(board, act, showId);

        window()->execDialog(choose);
        choose->align(eAlignment::center);
    });
    setCity(nullptr);
}

void eCityButton::setValidator(const eCityValidator& v) {
    mValidator = v;
}

void eCityButton::setCity(const stdsptr<eWorldCity>& c) {
    const auto lcity = eLanguage::text("none");
    const auto ccname = c ? c->name() : lcity;
    setText(ccname);
    fitContent();
    mCity = c;
}
