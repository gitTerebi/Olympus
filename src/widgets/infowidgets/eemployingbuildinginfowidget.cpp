#include "eemployingbuildinginfowidget.h"

#include "buildings/eemployingbuilding.h"

#include "widgets/elabel.h"
#include "widgets/elayouthelpers.h"

#include "language.h"

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

    std::string blessStr;
    if(b->blessed()) {
        blessStr = Language::zeusText(69, 12); // "This building has been blessed!"
    } else if(b->cursed()) {
        blessStr = Language::zeusText(69, 10); // "Nothing functions properly while it's cursed!"
    }

    const int w = widgetWidth() - 2*p;
    const auto makeLbl = [&](const std::string& txt) {
        const auto lbl = new eLabel(txt, window());
        lbl->setFontSizeS();
        lbl->setPaddingS();
        lbl->setWrapWidth(w);
        lbl->fitContent();
        lbl->setWidth(w);
        lbl->setTextAlignment(Alignment::hcenter);
        return lbl;
    };

    const auto estr = std::to_string(b->employed());
    const auto mestr = std::to_string(b->maxEmployees());
    const auto emplStr = estr + " " + Language::zeusText(8, 13) +
                         "  (" + mestr + " " + Language::zeusText(69, 0);

    std::vector<eLayoutHelpers::eFlexItem> items;
    items.push_back({makeLbl(emplStr)});
    if(!preInfoStr.empty()) items.push_back({makeLbl(preInfoStr)});
    if(!infoStr.empty()) items.push_back({makeLbl(infoStr)});
    if(!blessStr.empty()) items.push_back({makeLbl(blessStr)});

    int contentH = p*(static_cast<int>(items.size()) - 1);
    for(const auto& i : items) {
        contentH += i.widget->height();
    }
    const int frameH = contentH + 4*p;
    const auto wid = addFramedWidget(frameH);

    const auto col = eLayoutHelpers::createFlexContainer(
                         window(), wid->width(), wid->height(),
                         eLayoutHelpers::eFlexDirection::column,
                         items,
                         {.gap = p,
                          .align = eLayoutHelpers::eAlign::center});
    wid->addWidget(col);
    (void)centerText; // always centered via flex container
}
