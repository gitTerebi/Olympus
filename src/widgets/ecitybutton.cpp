#include "ecitybutton.h"

#include "echoosecitydialog.h"
#include "main-window.h"
#include "engine/game-board.h"
#include "language.h"

void eCityButton::initialize(WorldBoard* const board,
                             const eCityAction& cact,
                             const bool showId) {
    mShowId = showId;
    setUnderline(false);
    setPressAction([this, board, cact]() {
        const auto choose = new eChooseCityDialog(window());
        choose->setValidator(mValidator);
        const auto act = [this, cact](const stdsptr<WorldCity>& c) {
            setCity(c);
            if(cact) cact(c);
        };
        choose->initialize(board, act, mShowId);

        window()->execDialog(choose);
        choose->align(Alignment::center);
    });
    setCity(nullptr);

    std::vector<std::string> cityNames;
    const auto& cities = board->cities();
    for(const auto& c : cities) {
        if(mValidator) {
            const bool v = mValidator(c);
            if(!v) continue;
        }
        const auto cname = showId ? c->nameWithId() : c->name();
        cityNames.push_back(cname);
    }
    if(cityNames.empty()) fitContent();
    else fitOptions(cityNames);
}

void eCityButton::setValidator(const eCityValidator& v) {
    mValidator = v;
}

void eCityButton::setCity(const stdsptr<WorldCity>& c) {
    const auto lcity = Language::text("none");
    const auto ccname = c ? (mShowId ? c->nameWithId() : c->name()) :
                            lcity;
    setText(ccname);
    mCity = c;
}
