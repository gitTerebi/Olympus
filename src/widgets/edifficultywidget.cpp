#include "edifficultywidget.h"

#include "elabel.h"
#include "elanguage.h"
#include "emainwindow.h"
#include "eupbutton.h"
#include "edownbutton.h"

namespace {
class eDifficultyTooltipWidget : public eWidget {
public:
    eDifficultyTooltipWidget(eMainWindow* const window) :
        eWidget(window) {}
    bool mouseMoveEvent(const eMouseEvent&) override { return true; }
};
}

void eDifficultyWidget::initialize(const Difficulty diff,
                                   const eChangeAction& act) {
    setNoPadding();

    const auto w = window();

    const auto diffText = eLanguage::zeusText(44, 219);
    const auto value = std::make_shared<Difficulty>(diff);

    const auto label = new eLabel(w);
    label->setNoPadding();
    label->setFontSizeS();

    const auto updateLabel = [label, diffText, value]() {
        const auto hdiff = DifficultyHelpers::name(*value);
        label->setText("  " + diffText + "  " + hdiff);
        label->fitContent();
    };
    updateLabel();

    const auto down = new eDownButton(w);
    addWidget(down);
    down->setPressAction([value, updateLabel, act]() {
        if(*value == Difficulty::beginner) return;
        const int diffi = static_cast<int>(*value);
        *value = static_cast<Difficulty>(diffi - 1);
        updateLabel();
        act(*value);
    });

    const auto up = new eUpButton(w);
    addWidget(up);
    up->setPressAction([value, updateLabel, act]() {
        if(*value == Difficulty::olympian) return;
        const int diffi = static_cast<int>(*value);
        *value = static_cast<Difficulty>(diffi + 1);
        updateLabel();
        act(*value);
    });

    addWidget(label);

    stackHorizontally();
    fitContent();
    label->align(eAlignment::vcenter);
}
