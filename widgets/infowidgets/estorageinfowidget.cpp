#include "estorageinfowidget.h"

#include "engine/egameboard.h"

#include "elanguage.h"

#include "../ebutton.h"

#include "../equestionwidget.h"

#include "emainwindow.h"

#include <algorithm>

class eDebugIconLabel : public eLabel
{
public:
    using eLabel::eLabel;

protected:
    void paintEvent(ePainter &p) override
    {
        eLabel::paintEvent(p);
        int w = 0;
        int h = 0;
        textureSize(w, h);
        const SDL_Rect texRect{(width() - w) / 2, (height() - h) / 2, w, h};
    }
};

class eResourceStorageStack : public eWidget
{
public:
    using eWidget::eWidget;

    void initialize(eStorageBuilding *const stor,
                    const std::vector<eResourceType> &types,
                    const eResourceType get,
                    const eResourceType empty,
                    const eResourceType accept,
                    std::map<eResourceType, eSwitchButton *> &buttons,
                    std::map<eResourceType, eSpinBox *> &spinBoxes,
                    const std::map<eResourceType, int> &maxCount,
                    const eAction &changed)
    {
        auto &board = stor->getBoard();

        const auto countW = new eWidget(window());
        const auto iconsW = new eWidget(window());
        const auto namesW = new eWidget(window());
        const auto buttonsW = new eWidget(window());
        const auto spinsW = new eWidget(window());
        const auto deleteW = new eWidget(window());

        countW->setNoPadding();
        iconsW->setNoPadding();
        namesW->setNoPadding();
        buttonsW->setNoPadding();
        spinsW->setNoPadding();
        deleteW->setNoPadding();

        const auto res = resolution();
        const double mult = res.multiplier();
        int iconHeight = 0;
        int iconsWidth = static_cast<int>(mult * 25);
        for (const auto type : types)
        {
            const auto tex = eResourceTypeHelpers::icon(res.uiScale(), type);
            if (tex)
            {
                iconHeight = std::max(iconHeight, tex->height());
                iconsWidth = std::max(iconsWidth, tex->width());
            }
        }
        const int rowHeight = std::max(static_cast<int>(mult * 23), iconHeight);
        const int countWidth = mult * 20;
        const int namesWidth = mult * 131;
        const int buttonsWidth = mult * 133;
        const int spinsWidth = mult * 80;
        const int deleteWidth = mult * 20;

        buttonsW->setWidth(namesWidth);

        for (const auto type : types)
        {
            const auto count = new eLabel(window());
            count->setFontSizeS();
            const int c = stor->count(type);
            count->setText(std::to_string(c));
            count->fitContent();
            count->setHeight(rowHeight);

            const auto icon = new eDebugIconLabel(window());
            const auto tex = eResourceTypeHelpers::icon(
                res.uiScale(), type);
            icon->setTexture(tex);
            icon->fitContent();
            icon->setHeight(rowHeight);
            icon->setVisible(true);

            const auto nameStr = eResourceTypeHelpers::typeName(type);
            eLabel *n = nullptr;
            if (board.editorMode())
            {
                const auto button = new eButton(window());
                button->setPressAction([stor, type, count, icon]()
                                       {
                    stor->add(type, 1);
                    const int c = stor->count(type);
                    count->setText(std::to_string(c));
                    icon->setVisible(c > 0); });
                n = button;
            }
            else
            {
                n = new eLabel(window());
            }
            n->setFontSizeS();
            n->setText(nameStr);
            n->fitContent();
            n->setHeight(rowHeight);

            const auto b = new eSwitchButton(window());
            b->setUnderline(false);

            b->setSwitchAction([b, changed](const int i)
                               {
                if(i == 0 || i == 3) {
                    b->setDarkFontColor();
                } else {
                    b->setLightFontColor();
                }
                changed(); });

            b->setFontSizeS();
            b->addValue(eLanguage::zeusText(130, 1)); // don't accept
            b->addValue(eLanguage::zeusText(130, 0)); // accept
            b->addValue(eLanguage::zeusText(130, 2)); // get
            b->addValue(eLanguage::zeusText(130, 3)); // empty
            b->fitContent();
            b->setHeight(rowHeight);

            buttons[type] = b;

            const auto s = new eSpinBox(window());
            s->setHeight(rowHeight);
            s->setWidth(spinsWidth);
            s->initialize(changed);
            s->label()->setFontSizeS();
            const int space = stor->spaceCount();
            if (type == eResourceType::sculpture)
            {
                s->setRange(1, space);
            }
            else
            {
                s->setRange(8, 4 * space);
                s->setIncrement(8);
            }
            s->setValue(maxCount.at(type));

            spinBoxes[type] = s;

            const auto del = new eButton(window());
            del->setText("x");
            del->setFontSizeS();
            del->setWidth(deleteWidth);
            del->setHeight(rowHeight);
            del->setPressAction([this, stor, type, count, icon, changed]()
                                {
                const int current = stor->count(type);
                if(current == 0) return;
                const auto q = new eQuestionWidget(window());
                const auto acceptA = [stor, type, count, icon, changed]() {
                    const int current = stor->count(type);
                    stor->take(type, current);
                    count->setText("0");
                    icon->setVisible(false);
                    if(changed) changed();
                };
                const std::string typeName = eResourceTypeHelpers::typeName(type);
                std::string msg = "Delete all " + typeName + "?";
                q->initialize("Confirm Delete", msg, acceptA, nullptr);
                window()->execDialog(q);
                q->align(eAlignment::center); });
            deleteW->addWidget(del);
            del->align(eAlignment::left);

            countW->addWidget(count);
            iconsW->addWidget(icon);
            namesW->addWidget(n);
            buttonsW->addWidget(b);
            spinsW->addWidget(s);

            icon->align(eAlignment::left);
            n->align(eAlignment::left);
            b->align(eAlignment::hcenter);

            if (static_cast<bool>(get & type))
            {
                b->setValue(2);
            }
            else if (static_cast<bool>(empty & type))
            {
                b->setValue(3);
                b->setDarkFontColor();
            }
            else if (static_cast<bool>(accept & type))
            {
                b->setValue(1);
            }
            else
            {
                b->setValue(0);
                b->setDarkFontColor();
            }
        }

        eFramedButton *resetBtn = nullptr;
        {
            const auto it = buttons.begin();
            const auto b0 = it->second;
            const int w = b0->width();
            const int h = b0->height();

            const auto b = new eFramedButton(window());
            b->setUnderline(false);
            b->setFontSizeS();
            b->setText(eLanguage::zeusText(130, 1));
            b->setDarkFontColor();
            b->resize(w, h);
            b->setPressAction([changed, buttons]()
                              {
                for(const auto it : buttons) {
                    const auto b = it.second;
                    b->setValue(0);
                    b->setDarkFontColor();
                }
                changed(); });
            const auto spacerB = new eWidget(window());
            spacerB->resize(1, mult * 8);
            buttonsW->addWidget(spacerB);
            buttonsW->addWidget(b);
            b->align(eAlignment::hcenter);

            const auto spacerS = new eWidget(window());
            spacerS->resize(1, mult * 8);
            spinsW->addWidget(spacerS);

            const auto be = new eFramedButton(window());
            be->setUnderline(false);
            be->setFontSizeS();
            be->setText("Reset");
            be->setWidth(spinsWidth);
            be->setHeight(h);
            be->setPressAction([changed, spinBoxes]()
                               {
                for(const auto& it : spinBoxes) {
                    it.second->setValue(it.second->max());
                }
                changed(); });
            spinsW->addWidget(be);
            resetBtn = be;

            const auto spacerN = new eWidget(window());
            spacerN->resize(1, mult * 8);
            namesW->addWidget(spacerN);

            const auto emptyBtn = new eFramedButton(window());
            emptyBtn->setUnderline(false);
            emptyBtn->setFontSizeS();
            emptyBtn->setText("Empty");
            emptyBtn->resize(w, h);
            emptyBtn->setPressAction([changed, buttons]()
                                     {
                for(const auto it : buttons) {
                    const auto b = it.second;
                    b->setValue(3);
                    b->setDarkFontColor();
                }
                changed(); });
            namesW->addWidget(emptyBtn);

            const auto spacerD = new eWidget(window());
            spacerD->resize(1, mult * 8);
            deleteW->addWidget(spacerD);
        }

        countW->stackVertically();
        iconsW->stackVertically();
        namesW->stackVertically();
        buttonsW->stackVertically();
        spinsW->stackVertically();
        deleteW->stackVertically();

        const int h = types.size() * rowHeight;
        countW->setHeight(h);
        iconsW->setHeight(h);
        const int extraH = mult * 8 + rowHeight;
        namesW->setHeight(h + extraH);
        buttonsW->setHeight(h + extraH);
        spinsW->setHeight(h + extraH);
        deleteW->setHeight(h + extraH);

        countW->setWidth(countWidth);
        iconsW->setWidth(iconsWidth);
        namesW->setWidth(namesWidth);
        buttonsW->setWidth(buttonsWidth);
        spinsW->setWidth(spinsWidth);
        deleteW->setWidth(deleteWidth);
        if (resetBtn)
            resetBtn->setX(spinsWidth - resetBtn->width());

        addWidget(countW);
        addWidget(iconsW);
        addWidget(namesW);
        addWidget(buttonsW);
        addWidget(spinsW);
        addWidget(deleteW);

        stackHorizontally();
        setNoPadding();
        fitContent();
    }
};

eStorageInfoWidget::eStorageInfoWidget(
    eMainWindow *const window, eMainWidget *const mw) : eEmployingBuildingInfoWidget(window, mw, true, false) {}

void eStorageInfoWidget::initialize(eStorageBuilding *const stor)
{
    const auto title = eBuilding::sNameForBuilding(stor);

    eEmployingBuildingInfoWidget::initialize(title, stor);

    eResourceType get;
    eResourceType empty;
    eResourceType accept;
    stor->getOrders(get, empty, accept);
    const auto all = stor->canAccept();
    const auto &maxCount = stor->maxCount();

    const auto stWid = centralWidget();

    const auto types = eResourceTypeHelpers::extractResourceTypes(all);

    const auto r = new eResourceStorageStack(window());
    const auto changed = [this, stor]()
    {
        std::map<eResourceType, int> maxCount;
        eResourceType get;
        eResourceType empty;
        eResourceType accept;
        eResourceType dontaccept;
        this->get(get, empty, accept, dontaccept, maxCount);
        stor->setOrders(get, empty, accept);
        stor->setMaxCount(maxCount);
    };
    r->initialize(stor, types, get, empty, accept,
                  mButtons, mSpinBoxes, maxCount, changed);
    stWid->addWidget(r);
    r->align(eAlignment::center);
}

void eStorageInfoWidget::get(eResourceType &get,
                             eResourceType &empty,
                             eResourceType &accept,
                             eResourceType &dontaccept,
                             std::map<eResourceType, int> &count) const
{
    get = eResourceType::none;
    empty = eResourceType::none;
    accept = eResourceType::none;
    dontaccept = eResourceType::none;
    for (const auto b : mButtons)
    {
        const auto type = b.first;
        const int val = b.second->currentValue();
        if (val == 0)
        {
            dontaccept = dontaccept | type;
        }
        else if (val == 1)
        {
            accept = accept | type;
        }
        else if (val == 2)
        {
            get = get | type;
        }
        else if (val == 3)
        {
            empty = empty | type;
        }
    }
    for (const auto s : mSpinBoxes)
    {
        const auto type = s.first;
        const int val = s.second->value();
        count[type] = val;
    }
}
