#include "emonsterattackeventwidget.h"

#include "widgets/eswitchbutton.h"

#include "widgets/elabeledwidget.h"
#include "widgets/emonsterbutton.h"
#include "elanguage.h"
#include "epointeventbasewidget.h"

void eMonsterAttackEventWidget::initialize(eMonsterInvasionEventBase* const e) {
    const int p = padding();

    const auto iniT = e->type();
    const auto monsterButtonL = new eLabeledWidget(window());
    const auto act = [e](const eMonsterType type) {
        e->setType(type);
    };
    const auto et = e->eGameEvent::type();
    const bool withGodsOnly = et == eGameEventType::monsterUnleashed;
    const auto monsterButton = new eMonsterButton(window());
    monsterButton->initialize(act, withGodsOnly, withGodsOnly);
    monsterButton->setType(iniT);
    monsterButtonL->setup(eLanguage::text("monster:"), monsterButton);
    addWidget(monsterButtonL);

    ePointEventBaseWidget::add(this, e);

    const auto aggressivnessButton = new eSwitchButton(window());
    aggressivnessButton->setUnderline(false);
    aggressivnessButton->addValue(eLanguage::zeusText(94, 0));
    aggressivnessButton->addValue(eLanguage::zeusText(94, 1));
    aggressivnessButton->addValue(eLanguage::zeusText(94, 2));
    aggressivnessButton->addValue(eLanguage::zeusText(94, 3));
    aggressivnessButton->fitValidContent();
    const auto a = e->aggressivness();
    const int v = static_cast<int>(a);
    aggressivnessButton->setValue(v);
    aggressivnessButton->setSwitchAction([e](const int v) {
        const auto a = static_cast<eMonsterAggressivness>(v);
        e->setAggressivness(a);
    });
    addWidget(aggressivnessButton);

    stackVertically(p);
    setNoPadding();
    fitContent();
}
