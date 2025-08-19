#include "einvasioneventwidget.h"

#include "engine/egameboard.h"

#include "elanguage.h"
#include "widgets/eswitchbutton.h"

void eInvasionEventWidget::initialize(eInvasionEvent* const e) {
    const int p = padding();

    const auto hardcodedButton = new eSwitchButton(window());
    hardcodedButton->setUnderline(false);
    hardcodedButton->addValue(eLanguage::text("hardcoded"));
    hardcodedButton->addValue(eLanguage::text("automatic"));
    hardcodedButton->fitContent();
    hardcodedButton->setSwitchAction([this, e](const int h) {
        mHardcoded = h == 0;
        e->setHardcoded(mHardcoded);
    });
    mHardcoded = e->hardcoded();
    hardcodedButton->setValue(mHardcoded ? 0 : 1);
    addWidget(hardcodedButton);

    stackVertically(p);
    fitContent();
}
