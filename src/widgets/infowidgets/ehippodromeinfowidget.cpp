#include "ehippodromeinfowidget.h"

#include "buildings/ehippodrome.h"

void eHippodromeInfoWidget::initialize(eHippodrome * const h) {
    eInfoWidget::initialize(Language::zeusText(167, 0));

    if(h->racing()) {
        addText(Language::zeusText(167, 1));
    }
    const bool closed = h->closed();
    if(!closed) {
        addText(Language::zeusText(167, 2));
    }
    const int l = h->length();
    const bool working = h->working();
    if(working) {
        if(l < 8) {
            addText(Language::zeusText(167, 3));
        } else if(l < 16) {
            addText(Language::zeusText(167, 4));
        } else if(l < 32) {
            addText(Language::zeusText(167, 6));
        } else if(l < 64) {
            addText(Language::zeusText(167, 8));
        } else if(l < 128) {
            addText(Language::zeusText(167, 9));
        } else {
            addText(Language::zeusText(167, 10));
        }
    }

    const auto lStr = std::to_string(l);
    addText(Language::zeusText(167, 12) + " " + lStr + " " + Language::zeusText(167, 13));

    if(closed) {
        if(working) {
            const int d = h->drachmasPerMonth();
            const auto dStr = std::to_string(d);
            addText(Language::zeusText(167, 14) + " " + dStr + " " + Language::zeusText(167, 15));
        }

        const int has = h->hasHorses();
        const int needs = h->neededHorses();
        const auto hasStr = std::to_string(has);
        if(has < needs) {
            const auto needsStr = std::to_string(needs);
            addText(Language::zeusText(167, 18) + " " + needsStr + " " + Language::zeusText(167, 19));
            addText(hasStr + " " + Language::zeusText(167, 20));
        } else {
            addText(hasStr + " " + Language::zeusText(167, 21));
        }
    }
}
