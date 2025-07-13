#include "eeditorsettingsmenu.h"

#include "eframedbutton.h"
#include "elanguage.h"

#include "characters/gods/egod.h"
#include "egodselectionwidget.h"
#include "emonsterselectionwidget.h"

#include "emainwindow.h"

#include "widgets/eeventselectionwidget.h"
#include "widgets/eepisodegoalselectionwidget.h"
#include "gameEvents/egameevent.h"
#include "widgets/echeckablebutton.h"
#include "widgets/edatebutton.h"
#include "widgets/evaluebutton.h"

#include "engine/ecampaign.h"
#include "evectorhelpers.h"

void eEditorSettingsMenu::initialize(const bool first,
                                     eCampaign* const c,
                                     eEpisode* const ep,
                                     eWorldBoard* const board) {
    setType(eFrameType::message);

    if(first) {
        const auto dateButt = new eDateButton(window());
        dateButt->initialize();
        dateButt->setDate(c->date());
        dateButt->setUnderline(false);
        dateButt->fitContent();
        dateButt->setDateChangeAction([c](const eDate& date) {
            c->setDate(date);
        });
        addWidget(dateButt);
        dateButt->align(eAlignment::hcenter);

        const auto fundsButt = new eFramedButton(window());
        fundsButt->setText(eLanguage::zeusText(44, 39));
        fundsButt->fitContent();
        fundsButt->setUnderline(false);

        fundsButt->setPressAction([this, ep, c]() {
            const auto fundsMenu = new eFramedWidget(window());
            fundsMenu->setType(eFrameType::message);
            fundsMenu->resize(width(), height());

            const auto& board = ep->fBoard;
            const auto pids = board->playersOnBoard();
            for(const auto& pid : pids) {
                const auto cid = board->playerCapital(pid);
                const auto name = board->cityName(cid);
                const auto fundsButt = new eValueButton(window());
                fundsButt->initialize(0, 99999);
                const int f = c->initialFunds(pid);
                const auto fStr = std::to_string(f);
                fundsButt->setValue(f);
                fundsButt->setText(name + " " + std::to_string(99999));
                fundsButt->setUnderline(false);
                fundsButt->fitContent();
                fundsButt->setText(name + " " + fStr);
                fundsButt->setValueChangeAction([c, fundsButt, pid, name](const int funds) {
                    const auto fStr = std::to_string(funds);
                    fundsButt->setText(name + " " + fStr);
                    c->setInitialFunds(pid, funds);
                });
                fundsMenu->addWidget(fundsButt);
                fundsButt->align(eAlignment::hcenter);
            }

            window()->execDialog(fundsMenu);
            fundsMenu->align(eAlignment::center);
            fundsMenu->layoutVertically();
        });
        addWidget(fundsButt);
        fundsButt->align(eAlignment::hcenter);

        const auto pricesAct = [this, c]() {
            const auto priceMenu = new eFramedWidget(window());
            priceMenu->setType(eFrameType::message);
            priceMenu->resize(width(), height());

            eWidget* column = nullptr;
            int i = 1;
            const auto addW = [&](eWidget* const w) {
                if(i > 5 || !column) {
                    if(column) {
                        column->stackVertically();
                        column->fitContent();
                        column->align(eAlignment::vcenter);
                    }
                    column = new eWidget(window());
                    priceMenu->addWidget(column);
                    i = 0;
                } else {
                    i++;
                }
                column->addWidget(w);
            };

            const auto res = resolution();
            const auto uiScale = res.uiScale();
            auto& prices = c->prices();
            std::map<eResourceType, eValueButton*> buttons;
            for(const auto& p : prices) {
                const auto w = new eWidget(window());
                w->setNoPadding();

                const auto type = p.first;
                const auto icon = eResourceTypeHelpers::icon(uiScale, type);
                const auto iconLabel = new eLabel(window());
                iconLabel->setTexture(icon);
                iconLabel->fitContent();
                w->addWidget(iconLabel);

                const auto priceButt = new eValueButton(window());
                buttons[type] = priceButt;
                priceButt->initialize(0, 99999);
                const int f = p.second;
                const auto fStr = std::to_string(f);
                priceButt->setValue(f);
                priceButt->setText(fStr);
                priceButt->setUnderline(false);
                priceButt->fitContent();
                priceButt->setValueChangeAction([c, type](const int price) {
                    auto& prices = c->prices();
                    prices[type] = price;
                });
                w->addWidget(priceButt);
                w->stackHorizontally();
                w->fitContent();

                addW(w);
            }

            if(column) {
                const auto resetButt = new eFramedButton(window());
                resetButt->setUnderline(false);
                resetButt->setText(eLanguage::zeusText(44, 214));
                resetButt->fitContent();
                resetButt->setPressAction([c, buttons]() {
                    auto& prices = c->prices();
                    for(auto& p : prices) {
                        const auto type = p.first;
                        const int dprice = eResourceTypeHelpers::defaultPrice(type);
                        prices[type] = dprice;
                        const auto butt = buttons.find(type);
                        if(butt != buttons.end()) {
                            butt->second->setValue(dprice);
                        }
                    }
                });
                column->addWidget(resetButt);

                column->stackVertically();
                column->fitContent();
                column->align(eAlignment::vcenter);
            }

            window()->execDialog(priceMenu);
            priceMenu->align(eAlignment::center);
            priceMenu->layoutHorizontally();
        };

        const auto pricesButt = new eFramedButton(window());
        pricesButt->setUnderline(false);
        pricesButt->setText(eLanguage::zeusText(54, 9));
        pricesButt->fitContent();
        pricesButt->setPressAction(pricesAct);
        addWidget(pricesButt);
        pricesButt->align(eAlignment::hcenter);
    }

    const auto mythologyAct = [this, ep, board]() {
        const auto mythMenu = new eFramedWidget(window());
        mythMenu->setType(eFrameType::message);
        mythMenu->resize(width(), height());

        const auto cids = ep->fBoard->citiesOnBoard();
        for(const auto& cid : cids) {
            const auto friendGodsAct = [this, ep, cid]() {
                const auto choose = new eGodSelectionWidget(window());
                const auto act = [ep, cid](const std::vector<eGodType>& godNs) {
                    ep->fFriendlyGods[cid] = godNs;
                };

                const auto& iniG = ep->fFriendlyGods[cid];
                choose->resize(width(), height());
                choose->initialize(act, iniG);

                window()->execDialog(choose);
                choose->align(eAlignment::center);
            };

            const auto friendGodsButt = new eFramedButton(window());
            friendGodsButt->setUnderline(false);
            const auto name = board->cityName(cid);
            friendGodsButt->setText(name);
            friendGodsButt->fitContent();
            friendGodsButt->setPressAction(friendGodsAct);
            mythMenu->addWidget(friendGodsButt);
            friendGodsButt->align(eAlignment::hcenter);
        }

        window()->execDialog(mythMenu);
        mythMenu->align(eAlignment::center);
        mythMenu->layoutVertically();
    };

    const auto mythButt = new eFramedButton(window());
    mythButt->setUnderline(false);
    mythButt->setText(eLanguage::zeusText(44, 162));
    mythButt->fitContent();
    mythButt->setPressAction(mythologyAct);
    addWidget(mythButt);
    mythButt->align(eAlignment::hcenter);

    const auto eventsAct = [this, ep, board]() {
        const auto mythMenu = new eFramedWidget(window());
        mythMenu->setType(eFrameType::message);
        mythMenu->resize(width(), height());

        const auto cids = ep->fBoard->citiesOnBoard();
        for(const auto cid : cids) {
            const auto eventsAct = [this, ep, cid]() {
                const auto choose = new eEventSelectionWidget(
                                        eGameEventBranch::root,
                                        window());

                choose->resize(width(), height());
                const auto get = [ep, cid]() {
                    return ep->fEvents[cid];
                };
                const auto add = [ep, cid](const stdsptr<eGameEvent>& e) {
                    ep->fEvents[cid].push_back(e);
                };
                const auto remove = [ep, cid](const stdsptr<eGameEvent>& e) {
                    eVectorHelpers::remove(ep->fEvents[cid], e);
                };
                auto& board = *ep->fBoard;
                choose->initialize(cid, get, add, remove, board);

                window()->execDialog(choose);
                choose->align(eAlignment::center);
            };

            const auto friendGodsButt = new eFramedButton(window());
            friendGodsButt->setUnderline(false);
            const auto name = board->cityName(cid);
            friendGodsButt->setText(name);
            friendGodsButt->fitContent();
            friendGodsButt->setPressAction(eventsAct);
            mythMenu->addWidget(friendGodsButt);
            friendGodsButt->align(eAlignment::hcenter);
        }

        window()->execDialog(mythMenu);
        mythMenu->align(eAlignment::center);
        mythMenu->layoutVertically();
    };

    const auto eventsButt = new eFramedButton(window());
    eventsButt->setUnderline(false);
    eventsButt->setText(eLanguage::zeusText(44, 94));
    eventsButt->fitContent();
    eventsButt->setPressAction(eventsAct);
    addWidget(eventsButt);
    eventsButt->align(eAlignment::hcenter);

    const auto goalsAct = [this, ep, board]() {
        const auto choose = new eEpisodeGoalSelectionWidget(
                                window());

        choose->resize(width(), height());
        const auto get = [ep]() {
            return ep->fGoals;
        };
        const auto add = [ep](const stdsptr<eEpisodeGoal>& e) {
            ep->fGoals.push_back(e);
        };
        const auto remove = [ep](const stdsptr<eEpisodeGoal>& e) {
            eVectorHelpers::remove(ep->fGoals, e);
        };
        choose->initialize(get, add, remove, board);

        window()->execDialog(choose);
        choose->align(eAlignment::center);
    };

    const auto goalsButt = new eFramedButton(window());
    goalsButt->setUnderline(false);
    goalsButt->setText(eLanguage::zeusText(44, 45));
    goalsButt->fitContent();
    goalsButt->setPressAction(goalsAct);
    addWidget(goalsButt);
    goalsButt->align(eAlignment::hcenter);

    const auto buildingsAct = [this, ep]() {
        const auto buildMenu = new eFramedWidget(window());
        buildMenu->setType(eFrameType::message);
        buildMenu->resize(width(), height());

        const auto board = ep->fBoard;
        const auto cids = board->citiesOnBoard();
        for(const auto cid : cids) {
            const auto c = board->boardCityWithId(cid);
            if(!c) continue;
            const auto ab = &ep->fAvailableBuildings[cid];

            const auto cbuildingsAct = [this, ab]() {
                const auto buildMenu = new eFramedWidget(window());
                buildMenu->setType(eFrameType::message);
                buildMenu->resize(width(), height());

                const std::vector<eBuildingType> bv {
                    eBuildingType::eliteHousing,

                    eBuildingType::wheatFarm,
                    eBuildingType::carrotsFarm,
                    eBuildingType::onionsFarm,

                    eBuildingType::vine,
                    eBuildingType::oliveTree,
                    eBuildingType::orangeTree,

                    eBuildingType::dairy,
                    eBuildingType::cardingShed,

                    eBuildingType::fishery,
                    eBuildingType::urchinQuay,
                    eBuildingType::huntingLodge,
                    eBuildingType::corral,

                    eBuildingType::mint,
                    eBuildingType::foundry,
                    eBuildingType::timberMill,
                    eBuildingType::masonryShop,

                    eBuildingType::winery,
                    eBuildingType::olivePress,
                    eBuildingType::sculptureStudio,

                    eBuildingType::armory,

                    eBuildingType::horseRanch,
                    eBuildingType::chariotFactory,
                };

                int w = 0;
                std::vector<eCheckableButton*> buttons;
                for(const auto& type : bv) {
                    const auto bb = new eCheckableButton(window());
                    bb->setSmallFontSize();
                    bb->setSmallPadding();
                    bb->setText(eBuilding::sNameForBuilding(type));
                    bb->fitContent();
                    w = std::max(w, bb->width());
                    bb->setChecked(ab->available(type));
                    bb->setCheckAction([type, ab](const bool b) {
                        if(b) {
                            ab->allow(type);
                        } else {
                            ab->disallow(type);
                        }
                    });
                    buttons.push_back(bb);
                    buildMenu->addWidget(bb);
                }
                const int p = padding();
                int x = 2*p;
                int y = 2*p;
                for(const auto b : buttons) {
                    b->setWidth(w);
                    const int bh = b->height();
                    if(y + bh + 2*p > buildMenu->height()) {
                        x += w;
                        y = 2*p;
                    }
                    b->move(x, y);
                    y += bh + p;
                }

                window()->execDialog(buildMenu);
                buildMenu->align(eAlignment::center);
            };
            const auto cButt = new eFramedButton(window());
            cButt->setUnderline(false);
            cButt->setText(board->cityName(cid));
            cButt->fitContent();
            cButt->setPressAction(cbuildingsAct);
            buildMenu->addWidget(cButt);
            cButt->align(eAlignment::hcenter);
        }
        window()->execDialog(buildMenu);
        buildMenu->align(eAlignment::center);
        buildMenu->layoutVertically();
    };

    const auto buildingsButt = new eFramedButton(window());
    buildingsButt->setUnderline(false);
    buildingsButt->setText(eLanguage::zeusText(44, 44));
    buildingsButt->fitContent();
    buildingsButt->setPressAction(buildingsAct);
    addWidget(buildingsButt);
    buildingsButt->align(eAlignment::hcenter);

    layoutVertically();
}
