#include "ecityonboardselectionwidget.h"

#include "widgets/echoosebutton.h"
#include "emainwindow.h"
#include "engine/eboardcity.h"
#include "ecitysettingswidget.h"
#include "engine/egameboard.h"
#include "ecitybutton.h"
#include "echoosecitydialog.h"

void eCityOnBoardSelectionWidget::initialize(
        const eCitiesGetter& get,
        const eCityAdder& add,
        const eCityRemover& remove,
        eGameBoard* const board,
        eWorldBoard* const wboard) {
    eScrollButtonsList::initialize();

    const auto iniCids = get();
    for(const auto cid : iniCids) {
        const auto cStr = wboard->cityName(cid);
        addButton(cStr);
    }

    const auto editCity = [this, board, wboard](const eCityId cid) {
        const auto settings = new eCitySettingsWidget(window());
        settings->resize(2*width()/3, 2*height()/3);
        const auto wc = wboard->cityWithId(cid);
        settings->initialize(wc, board, wboard);

        window()->execDialog(settings);
        settings->align(eAlignment::center);
    };

    setButtonPressEvent([get, editCity](const int id) {
        const auto cids = get();
        const auto cid = cids[id];
        editCity(cid);
    });

    setButtonCreateEvent([this, add, wboard]() {
        const auto choose = new eChooseCityDialog(window());
        const auto act = [this, add](const stdsptr<eWorldCity>& wc) {
            const auto cid = wc->cityId();
            add(cid);
            addButton(wc->name());
        };
        choose->initialize(wboard, act);

        window()->execDialog(choose);
        choose->align(eAlignment::center);
    });

    setButtonRemoveEvent([get, remove](const int id) {
        const auto cs = get();
        const auto& c = cs[id];
        remove(c);
    });
}
