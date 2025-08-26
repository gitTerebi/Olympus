#include "etriremewharfinfowidget.h"

#include "buildings/etriremewharf.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "widgets/eswitchbutton.h"

void eTriremeWharfInfoWidget::initialize(eTriremeWharf* const b) {
    std::string title;
    std::string info;
    std::string employmentInfo;
    std::string additionalInfo;
    eBuilding::sInfoText(b, title, info, employmentInfo, additionalInfo);
    eInfoWidget::initialize(title);

    auto& board = b->getBoard();
    const auto cid = b->cityId();
    const bool palace = board.hasPalace(cid);
    if(!palace) {
        addText(eLanguage::zeusText(175, 18));
    }

    const auto storedWood = eLanguage::zeusText(175, 12);
    const auto storedArmor = eLanguage::zeusText(175, 13);

    const int cwood = b->count(eResourceType::wood);
    const int carmor = b->count(eResourceType::armor);

    const auto cwoodStr = std::to_string(cwood);
    const auto carmorStr = std::to_string(carmor);

    const auto loads = eLanguage::zeusText(8, 55);

    addText(storedWood + " " + cwoodStr + " " + loads + "  " +
            storedArmor + " " + carmorStr + " " + loads);

    const bool r = b->accessToRoad();
    if(!r) {
        addText(eLanguage::zeusText(69, 4));
    }

    addText(info);

    addText(employmentInfo);

    addEmploymentWidget(b);

    addText(additionalInfo);

    const auto button = new eSwitchButton(window());
    button->setUnderline(false);
    button->addValue(eLanguage::zeusText(175, 19));
    button->addValue(eLanguage::zeusText(175, 20));
    button->fitValidContent();
    button->setValue(b->shutDown() ? 0 : 1);
    button->setSwitchAction([b](const int val) {
        b->setShutDown(val == 0);
    });
    const int bheight = button->height();
    const auto w = addRegularWidget(bheight);
    w->addWidget(button);
    button->align(eAlignment::hcenter);
}
