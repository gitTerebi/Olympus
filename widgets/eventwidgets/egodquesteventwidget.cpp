#include "egodquesteventwidget.h"

#include "widgets/eswitchbutton.h"

#include "widgets/elabeledwidget.h"
#include "widgets/egodbutton.h"
#include "widgets/eherobutton.h"
#include "elanguage.h"

void eGodQuestEventWidget::initialize(
        eGodQuestEventBase* const e) {
    mE = e;

    const auto godButtonL = new eLabeledWidget(window());
    const auto act = [this, e](const eGodType type) {
        e->setGod(type);
        setDefaultHero();
    };
    const auto godButton = new eGodButton(window());
    godButton->initialize(act);
    const auto iniT = e->god();
    godButton->setType(iniT);
    godButtonL->setup(eLanguage::zeusText(44, 391), godButton);
    addWidget(godButtonL);

    const auto idButtonL = new eLabeledWidget(window());
    const auto switchAct = [this, e](const int id) {
        const auto eid = id == 0 ? eGodQuestId::godQuest1 :
                                   eGodQuestId::godQuest2;
        e->setId(eid);
        setDefaultHero();
    };
    const auto idButton = new eSwitchButton(window());
    idButton->setUnderline(false);
    idButton->addValue("1");
    idButton->addValue("2");
    idButton->fitValidContent();
    const auto iniId = e->id();
    idButton->setValue(iniId == eGodQuestId::godQuest1 ? 0 : 1);
    idButton->setSwitchAction(switchAct);
    idButtonL->setup(eLanguage::zeusText(44, 357), idButton);
    addWidget(idButtonL);

    const auto heroButtonL = new eLabeledWidget(window());
    const auto hact = [e](const eHeroType type) {
        e->setHero(type);
    };

    mHeroButton = new eHeroButton(window());
    mHeroButton->initialize(hact);
    const auto iniH = e->hero();
    mHeroButton->setType(iniH);
    heroButtonL->setup(eLanguage::zeusText(44, 267), mHeroButton);
    addWidget(heroButtonL);

    const int p = padding();
    stackVertically(p);
    setNoPadding();
    fitContent();
}

void eGodQuestEventWidget::setDefaultHero() {
    if(!mE) return;
    const auto id = mE->id();
    const auto godType = mE->god();
    const auto heroType = eGodQuest::sDefaultHero(godType, id);
    mHeroButton->setType(heroType);
    mE->setHero(heroType);
}
