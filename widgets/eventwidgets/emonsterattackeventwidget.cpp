#include "emonsterattackeventwidget.h"

#include "widgets/eframedbutton.h"
#include "widgets/echoosebutton.h"
#include "widgets/eswitchbutton.h"

#include "widgets/elabeledwidget.h"
#include "widgets/emonsterbutton.h"
#include "widgets/evaluebutton.h"
#include "elanguage.h"

#include "emainwindow.h"

void eMonsterAttackEventWidget::initialize(eMonsterInvasionEventBase* const e) {
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

    const auto invasionPointButtonL = new eLabeledWidget(window());
    const auto invasionPointButton = new eValueButton(window());
    invasionPointButton->setValueChangeAction([e](const int p) {
        e->setPointId(p - 1);
    });
    invasionPointButton->initialize(1, 999);
    invasionPointButton->setValue(e->pointId() + 1);
    invasionPointButtonL->setup(eLanguage::text("invasion_point:"), invasionPointButton);
    addWidget(invasionPointButtonL);

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

    const int p = padding();
    stackVertically(p);
    setNoPadding();
    fitContent();
}
