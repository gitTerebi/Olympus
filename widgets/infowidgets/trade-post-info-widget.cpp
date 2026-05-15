#include "trade-post-info-widget.h"

#include "textures/egametextures.h"
#include "engine/e-game-board.h"
#include "elanguage.h"
#include "enumbers.h"
#include "../eleftbutton.h"
#include "../erightbutton.h"
#include "../ebuttonbase.h"

class eResourceStorageStack : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eStorageBuilding* const stor,
                    const std::vector<eResourceTrade>& importTypes,
                    const eResourceType imports,
                    std::map<eResourceType, eSwitchButton*>& buttons,
                    std::map<eResourceType, eSpinBox*>& spinBoxes,
                    const std::map<eResourceType, int>& maxCount,
                    const std::string& notBuyingTxt,
                    const std::string& buyingTxt,
                    const eAction& changed,
                    const bool twoWay,
                    std::map<eResourceType, eSwitchButton*>* cartOrderButtons = nullptr,
                    const eResourceType cartEmpty = eResourceType::none,
                    const eResourceType cartGet = eResourceType::none) {
        const auto countW = new eWidget(window());
        const auto iconsW = new eWidget(window());
        const auto namesW = new eWidget(window());
        const auto priceW = new eWidget(window());
        const auto buttonsW = new eWidget(window());
        const auto cartOrderW = new eWidget(window());
        const auto spinsW = new eWidget(window());
        const auto tradedW = new eWidget(window());

        const auto res = resolution();
        const double mult = res.multiplier();
        const int rowHeight = mult*23;
        const int countWidth = mult*25;
        const int iconsWidth = mult*40;
        const int namesWidth = mult*110;
        const int priceWidth = mult*60;
        const int buttonsWidth = mult*110;
        const int cartOrderWidth = mult*70;
        const int spinsWidth = mult*90;
        const int tradedWidth = mult*80;

        priceW->setWidth(priceWidth);
        buttonsW->setWidth(buttonsWidth);
        cartOrderW->setWidth(cartOrderWidth);

        for(const auto& trade : importTypes) {
            const auto type = trade.fType;

            const auto count = new eLabel(window());
            count->setFontSizeS();
            const int c = stor->count(type);
            count->setText(std::to_string(c));
            count->fitContent();
            count->setHeight(rowHeight);

            const auto icon = new eLabel(window());
            const auto tex = eResourceTypeHelpers::icon(
                                 res.uiScale(), type);
            icon->setTexture(tex);
            icon->fitContent();
            icon->setHeight(rowHeight);

            const auto nameStr = eResourceTypeHelpers::typeName(type);
            const auto n = new eLabel(window());
//            const auto n = new eButton(window());
//            n->setPressAction([stor, type, count]() {
//                stor->add(type, 1);
//                const int c = stor->count(type);
//                count->setText(std::to_string(c));
//            });
            n->setFontSizeS();
            n->setText(nameStr);
            n->fitContent();
            n->setHeight(rowHeight);


            const auto pi = new eLabel(window());
            const auto& intrfs = eGameTextures::interface();
            int icoll;
            switch(res.uiScale()) {
            case eUIScale::tiny:
            case eUIScale::small:
                icoll = 1;
                break;
            default:
                icoll = 2;
            }
            pi->setTexture(intrfs[icoll].fDrachmasUnit);
            pi->setNoPadding();
            pi->fitContent();

            const auto p = new eLabel(window());
            auto& board = stor->getBoard();
            p->setText(std::to_string(board.price(trade.fType)));
            p->setFontSizeS();
            p->setNoPadding();
            p->fitContent();

            const auto pw = new eWidget(window());
            pw->setNoPadding();
            pw->addWidget(pi);
            pw->addWidget(p);
            pw->stackHorizontally();
            pw->fitContent();
            pw->setHeight(rowHeight);
            if(twoWay) pw->hide();

            pi->align(eAlignment::vcenter);
            p->align(eAlignment::vcenter);

            const auto b = new eSwitchButton(window());
            b->setUnderline(false);
            b->setSwitchAction([b, changed](const int i) {
                if(i == 0) {
                    b->setDarkFontColor();
                } else {
                    b->setLightFontColor();
                }
                changed();
            });

            b->setFontSizeS();
            b->addValue(notBuyingTxt);
            b->addValue(buyingTxt);
            b->fitContent();
            b->setHeight(rowHeight);

            buttons[type] = b;

            if(cartOrderButtons) {
                const auto cb = new eSwitchButton(window());
                cb->setUnderline(false);
                cb->setSwitchAction([cb, changed](const int i) {
                    if(i == 2) cb->setDarkFontColor();
                    else cb->setLightFontColor();
                    changed();
                });
                cb->setFontSizeS();
                cb->addValue(eLanguage::zeusText(130, 0)); // accept
                cb->addValue(eLanguage::zeusText(130, 2)); // get
                cb->addValue(eLanguage::zeusText(130, 3)); // empty
                cb->fitContent();
                cb->setHeight(rowHeight);
                if(static_cast<bool>(cartEmpty & type)) {
                    cb->setValue(2);
                    cb->setDarkFontColor();
                } else if(static_cast<bool>(cartGet & type)) {
                    cb->setValue(1);
                } else {
                    cb->setValue(0);
                }
                (*cartOrderButtons)[type] = cb;
                cartOrderW->addWidget(cb);
                cb->align(eAlignment::hcenter);
            }

            const auto s = new eSpinBox(window());
            s->setHeight(rowHeight);
            s->setWidth(spinsWidth);
            s->initialize(changed);
            s->label()->setFontSizeS();
            const int space = stor->spaceCount();
            if(type == eResourceType::sculpture) {
                s->setRange(0, space);
            } else {
                s->setRange(0, 4*space);
                s->setIncrement(4);
            }
            s->setValue(maxCount.at(type));

            spinBoxes[type] = s;

            const auto t = new eButtonBase(window());
            t->setFontSizeS();
            const auto pid = stor->playerId();
            const int cc = trade.used(pid);
            const int ccc = trade.fMax;
            t->setText(std::to_string(cc) + " " + eLanguage::zeusText(44, 148) + " " + std::to_string(ccc));
            t->fitContent();
            t->setHeight(rowHeight);
            t->setPressAction([s, ccc, changed]() {
                s->setValue(ccc);
                changed();
            });

            countW->addWidget(count);
            iconsW->addWidget(icon);
            namesW->addWidget(n);
            priceW->addWidget(pw);
            pw->align(eAlignment::hcenter);
            buttonsW->addWidget(b);
            spinsW->addWidget(s);
            tradedW->addWidget(t);

            icon->align(eAlignment::left);
            n->align(eAlignment::left);
            b->align(eAlignment::hcenter);

            if(static_cast<bool>(imports & type)) {
                b->setValue(1);
            } else {
                b->setValue(0);
                b->setDarkFontColor();
            }
        }

        countW->stackVertically();
        iconsW->stackVertically();
        namesW->stackVertically();
        priceW->stackVertically();
        buttonsW->stackVertically();
        cartOrderW->stackVertically();
        spinsW->stackVertically();
        tradedW->stackVertically();

        countW->fitContent();
        iconsW->fitContent();
        namesW->fitContent();
        priceW->fitContent();
        buttonsW->fitContent();
        cartOrderW->fitContent();
        spinsW->fitContent();
        tradedW->fitContent();

        countW->setWidth(countWidth);
        iconsW->setWidth(iconsWidth);
        namesW->setWidth(namesWidth);
        priceW->setWidth(priceWidth);
        buttonsW->setWidth(buttonsWidth);
        cartOrderW->setWidth(cartOrderWidth);
        spinsW->setWidth(spinsWidth);
        tradedW->setWidth(tradedWidth);

        addWidget(countW);
        addWidget(iconsW);
        addWidget(namesW);
        addWidget(priceW);
        addWidget(buttonsW);
        addWidget(cartOrderW);
        addWidget(spinsW);
        addWidget(tradedW);

        stackHorizontally(0);
        setNoPadding();
        fitContent();
    }
};

eTradePostInfoWidget::eTradePostInfoWidget(
        eMainWindow* const window, eMainWidget* const mw) :
    eEmployingBuildingInfoWidget(window, mw, false, false) {}

void eTradePostInfoWidget::initialize(eTradePost* const stor,
                                      const ePrevNextAction& prevNext) {
    const auto& city = stor->city();
    const auto title = eLanguage::zeusText(28, 62) + ": " + city.name();

    std::string sTitle;
    std::string sInfo;
    std::string sEmpl;
    std::string sAdd;
    eBuilding::sInfoText(stor, sTitle, sInfo, sEmpl, sAdd);

    eInfoWidget::initialize(title);
    addCentralWidget();
    addEmploymentWidget(stor, sEmpl);

    eResourceType imports;
    eResourceType exports;
    eResourceType cartEmpty;
    eResourceType cartGet;
    eResourceType cartAccept;
    stor->getOrders(imports, exports, cartEmpty, cartGet, cartAccept);
    const auto& maxCount = stor->maxCount();

    const auto stWid = centralWidget();

    const auto changed = [this, stor]() {
        std::map<eResourceType, int> maxCount;
        eResourceType imports;
        eResourceType exports;
        eResourceType empty;
        eResourceType cartGet;
        eResourceType cartAccept;
        this->get(imports, exports, empty, cartGet, cartAccept, maxCount);
        stor->setOrders(imports, exports, empty, cartGet, cartAccept);
        stor->setMaxCount(maxCount);
    };

    const bool twoWay = stor->playerTwoWay();

    if(!twoWay) {
        const auto wrapper = new eWidget(window());
        const auto importsLabel = new eLabel(eLanguage::zeusText(130, 17), window());
        importsLabel->fitContent();

        const auto r = new eResourceStorageStack(window());

        const auto& csells = city.sells();

        r->initialize(stor, csells, imports,
                      mImportButtons, mSpinBoxes, maxCount,
                      eLanguage::zeusText(130, 20),
                      eLanguage::zeusText(130, 12),
                      changed, twoWay);

        wrapper->addWidget(importsLabel);
        wrapper->addWidget(r);
        wrapper->stackVertically();
        wrapper->setNoPadding();
        wrapper->fitContent();

        stWid->addWidget(wrapper);
        wrapper->align(eAlignment::hcenter);
    }
    {
        const auto wrapper = new eWidget(window());
        const auto exportsLabel = new eLabel(eLanguage::zeusText(130, 18), window());
        exportsLabel->fitContent();

        const auto r = new eResourceStorageStack(window());

        std::vector<eResourceTrade> cbuys;
        if(twoWay) {
            const auto all = eResourceTypeHelpers::extractResourceTypes(
                                 eResourceType::allBasic);
            const auto thisCid = stor->cityId();
            const auto thisPid = stor->playerId();
            const auto& board = stor->getBoard();
            const auto thisC = board.boardCityWithId(thisCid);
            const auto dstCid = city.cityId();
            for(const auto r : all) {
                const int count = thisC->resourceCount(r);
                const bool ex = static_cast<bool>(exports & r);
                if(count < 1 && !ex) continue;
                const int e = thisC->exported(dstCid, r);
                const int max = r == eResourceType::sculpture ? eNumbers::sTwoWayTradeMax :
                                                               4*eNumbers::sTwoWayTradeMax;
                std::map<ePlayerId, int> em;
                em[thisPid] = e;
                cbuys.push_back(eResourceTrade{r, em, max});
            }
        } else {
            cbuys = city.buys();
        }

        r->initialize(stor, cbuys, exports,
                      mExportButtons, mSpinBoxes, maxCount,
                      eLanguage::zeusText(130, 21),
                      eLanguage::zeusText(130, 13),
                      changed, twoWay,
                      &mCartOrderButtons, cartEmpty, cartGet);

        wrapper->addWidget(exportsLabel);
        wrapper->addWidget(r);
        wrapper->stackVertically();
        wrapper->setNoPadding();
        wrapper->fitContent();

        stWid->addWidget(wrapper);
        wrapper->align(eAlignment::hcenter);
    }

    stWid->layoutVertically();

    if(prevNext) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        const auto prevButton = new eLeftButton(window());
        prevButton->setPressAction([prevNext]() {
            prevNext(false);
        });
        const auto nextButton = new eRightButton(window());
        nextButton->setPressAction([prevNext]() {
            prevNext(true);
        });
        w->addWidget(prevButton);
        w->addWidget(nextButton);
        w->stackHorizontally();
        w->fitContent();

        addWidget(w);
        const auto res = resolution();
        const int p = res.paddingL();
        w->move(width() - p - w->width(), p);
    }
}

void eTradePostInfoWidget::get(eResourceType& imports,
                               eResourceType& exports,
                               eResourceType& empty,
                               eResourceType& cartGet,
                               eResourceType& cartAccept,
                               std::map<eResourceType, int>& count) const {
    imports = eResourceType::none;
    exports = eResourceType::none;
    empty = eResourceType::none;
    cartGet = eResourceType::none;
    cartAccept = eResourceType::none;
    for(const auto b : mImportButtons) {
        const auto type = b.first;
        const int val = b.second->currentValue();
        if(val == 1) {
            imports = imports | type;
        }
    }
    for(const auto b : mExportButtons) {
        const auto type = b.first;
        const int val = b.second->currentValue();
        if(val == 1) {
            exports = exports | type;
        }
    }
    for(const auto b : mCartOrderButtons) {
        const auto type = b.first;
        const int val = b.second->currentValue();
        if(val == 1) {
            cartGet = cartGet | type;
        } else if(val == 0) {
            cartAccept = cartAccept | type;
        } else if(val == 2) {
            empty = empty | type;
        }
    }
    for(const auto s : mSpinBoxes) {
        const auto type = s.first;
        const int val = s.second->value();
        count[type] = val;
    }
}
