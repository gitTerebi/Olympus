#include "eemployingbuildinginfowidget.h"

#include "buildings/eemployingbuilding.h"

#include "widgets/elabel.h"

#include "elanguage.h"

void eEmployingBuildingInfoWidget::initialize(
        const std::string& title,
        eEmployingBuilding* const b) {
    eInfoWidget::initialize(title);
    addCentralWidget();
    addEmploymentWidget(b);
}

void eEmployingBuildingInfoWidget::initialize(
        const std::string& title,
        const std::string& info,
        const std::string& text,
        eEmployingBuilding* const b,
        const std::string& subText) {
    eInfoWidget::initialize(title);
    if(!info.empty()) addText(info);
    addEmploymentWidget(b, text, subText);
}

void eEmployingBuildingInfoWidget::addEmploymentWidget(
        eEmployingBuilding* const b,
        const std::string& infoStr,
        const std::string& preInfoStr) {
    const int p = padding();

    const bool hasInfo = !infoStr.empty();
    const bool hasPre = !preInfoStr.empty();
    const int frameH = (!hasInfo && !hasPre) ? 6*p : (hasInfo && hasPre) ? 20*p : 10*p;
    const auto wid = addFramedWidget(frameH);
    const int e = b->employed();
    const int me = b->maxEmployees();
    const auto estr = std::to_string(e);
    const auto mestr = std::to_string(me);
    const auto emplstr = eLanguage::zeusText(8, 13);
    const auto memplstr = eLanguage::zeusText(69, 0);
    const auto str = estr + " " + emplstr +
                      "  (" + mestr + " " + memplstr;
    const auto empl = new eLabel(str, window());
    empl->setFontSizeS();
    empl->setPaddingS();
    empl->fitContent();
    wid->addWidget(empl);
    if(!hasInfo && !hasPre) {
        empl->align(eAlignment::hcenter);
    } else {
        if(hasPre) {
            const auto pre = new eLabel(preInfoStr, window());
            pre->setFontSizeS();
            pre->setPaddingS();
            pre->setWrapWidth(wid->width());
            pre->fitContent();
            wid->addWidget(pre);
        }
        if(hasInfo) {
            const auto info = new eLabel(infoStr, window());
            info->setFontSizeS();
            info->setPaddingS();
            info->setWrapWidth(wid->width());
            info->fitContent();
            wid->addWidget(info);
        }
        wid->stackVertically(p);
        empl->align(eAlignment::hcenter);
    }
}
