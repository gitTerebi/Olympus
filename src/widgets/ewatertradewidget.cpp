#include "ewatertradewidget.h"

#include "widgets/echoosecitydialog.h"
#include "emainwindow.h"
#include "engine/eworldboard.h"
#include "evectorhelpers.h"

void eWaterTradeWidget::initialize(const eCitiesGetter& get,
                                   const eCityRemover& add,
                                   const eCityRemover& remove,
                                   eWorldBoard* const wboard) {
    eScrollButtonsList::initialize();

    const auto iniCids = get();
    for(const auto cid : iniCids) {
        const auto cStr = wboard->cityName(cid);
        addButton(cStr);
    }

    setButtonCreateEvent([this, get, add, wboard]() {
        const auto choose = new eChooseCityDialog(window());
        const auto cids = get();
        choose->setValidator([cids](const stdsptr<eWorldCity>& c) {
            const auto cid = c->cityId();
            return !eVectorHelpers::contains(cids, cid);
        });
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
        const auto cids = get();
        const auto& c = cids[id];
        remove(c);
    });
}
