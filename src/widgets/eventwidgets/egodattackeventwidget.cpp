#include "egodattackeventwidget.h"

#include "widgets/eswitchbutton.h"
#include "language.h"

#include "widgets/gods/egodselectionwidget.h"

#include "main-window.h"

void eGodAttackEventWidget::initialize(eWidget* const parent,
                                       eGodAttackEvent* const e) {
    const auto randomButton = new eSwitchButton(window());
    randomButton->addValue(Language::text("iterative"));
    randomButton->addValue(Language::text("random"));
    randomButton->setSwitchAction([e](const int v) {
        e->setRandom(v);
    });
    randomButton->fitValidContent();
    randomButton->setUnderline(false);
    randomButton->setValue(e->random() ? 1 : 0);
    addWidget(randomButton);

    const auto godsStr = Language::zeusText(44, 360);
    const auto godsButton = new FramedButton(godsStr, window());
    godsButton->fitContent();
    godsButton->setUnderline(false);
    godsButton->setPressAction([this, parent, e]() {
        const auto choose = new eGodSelectionWidget(window());
        const auto act = [e](const std::vector<GodType>& godNs) {
            e->setTypes(godNs);
        };

        choose->resize(parent->width(), parent->height());
        choose->initialize(act, e->types());

        window()->execDialog(choose);
        choose->align(Alignment::center);
    });
    addWidget(godsButton);

    const int p = padding();
    stackVertically(p);
    setNoPadding();
    fitContent();
}