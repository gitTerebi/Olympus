#include "eemployingbuildinginfowidget.h"

#include "buildings/eemployingbuilding.h"

#include "widgets/elabel.h"
#include "widgets/elayouthelpers.h"

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
        const std::string& preInfoStr,
        bool centerText) {
    const int p = padding();

    const bool hasInfo = !infoStr.empty();
    const bool hasPre = !preInfoStr.empty();
    const int frameH = (!hasInfo && !hasPre) ? 6*p : (hasInfo && hasPre) ? 20*p : 10*p;
    const auto wid = addFramedWidget(frameH);

    const auto makeLbl = [&](const std::string& txt) {
        const auto lbl = new eLabel(txt, window());
        lbl->setFontSizeS();
        lbl->setPaddingS();
        lbl->setWrapWidth(wid->width());
        lbl->fitContent();
        lbl->setWidth(wid->width());
        lbl->setTextAlignment(eAlignment::hcenter);
        return lbl;
    };

    const auto estr = std::to_string(b->employed());
    const auto mestr = std::to_string(b->maxEmployees());
    const auto emplStr = estr + " " + eLanguage::zeusText(8, 13) +
                         "  (" + mestr + " " + eLanguage::zeusText(69, 0);

    std::vector<eLayoutHelpers::eFlexItem> items;
    items.push_back({makeLbl(emplStr)});
    if(hasPre) items.push_back({makeLbl(preInfoStr)});
    if(hasInfo) items.push_back({makeLbl(infoStr)});

    const auto col = eLayoutHelpers::flexCol(
                         window(), wid->width(), frameH, items,
                         {.gap = p,
                          .align = eLayoutHelpers::eAlign::center});
    wid->addWidget(col);
    (void)centerText; // always centered via flexCol
}
