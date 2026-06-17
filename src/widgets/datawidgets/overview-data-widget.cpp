#include "overview-data-widget.h"

#include "eviewmodebutton.h"

#include "widgets/game-widget.h"
#include "widgets/framed-button.h"
#include "widgets/escrollwidget.h"
#include "widgets/esmallupbutton.h"

#include "language.h"
#include "engine/game-board.h"
#include "characters/soldier-banner.h"
#include "string-helpers.h"
#include "buildings/eheroshall.h"
#include "gameEvents/requests/receive-tribute-event.h"
#include "gameEvents/requests/send-resources-to-city-event.h"
#include "gameEvents/requests/send-troops-event.h"
#include "widgets/elinewidget.h"
#include "widgets/eminimap.h"

namespace {
std::string trimmedString(const std::string& str, const size_t maxLen = 10) {
    if (str.length() <= maxLen) return str;
    return str.substr(0, maxLen) + ".";
}

int scrollButtonGutter(MainWindow* const window) {
    const auto button = new eSmallUpButton(window);
    const int w = button->width();
    button->deleteLater();
    return w + 2;
}

void setChildWidths(eWidget* const widget, const int width) {
    for(const auto child : widget->children()) {
        child->setWidth(width);
    }
}

int stackedChildrenHeight(eWidget* const widget) {
    int h = 0;
    for(const auto child : widget->children()) {
        h += child->height();
    }
    return h;
}

void fitScrollAreaToButtons(eScrollWidget* const scroll,
                            eWidget* const scrollArea) {
    if(scrollArea->height() <= scroll->height()) return;
    const int w = scroll->width() - scrollButtonGutter(scroll->window());
    scrollArea->setWidth(w);
    setChildWidths(scrollArea, w);
}
}

class eOverviewEntry : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& title) {
        setNoPadding();
        mTitleLabel = new eLabel(window());
        mTitleLabel->setNoPadding();
        mTitleLabel->setFontSizeXS();
        mTitleLabel->setText(title);
        mTitleLabel->fitContent();
        addWidget(mTitleLabel);

        mValueLabel = new eLabel(window());
        mValueLabel->setYellowFontColor();
        mValueLabel->setNoPadding();
        mValueLabel->setFontSizeXS();
        addWidget(mValueLabel);

        fitHeight();
    }

    void setTitle(const std::string& title) {
        mTitleLabel->setText(title);
        mTitleLabel->fitContent();
    }

    void setText(const std::string& txt) {
        mValueLabel->setText(txt);
        mValueLabel->fitContent();
        mValueLabel->align(Alignment::right);
    }
private:
    eLabel* mTitleLabel = nullptr;
    eLabel* mValueLabel = nullptr;
};

void OverviewDataWidget::initialize() {
    mSeeProblems = new eViewModeButton(
                     Language::zeusText(14, 18),
                     eViewMode::problems,
                     window());
    addViewButton(mSeeProblems);

    mSeeRoads = new eViewModeButton(
                     Language::zeusText(14, 19),
                     eViewMode::roads,
                     window());
    addViewButton(mSeeRoads);

    eDataWidget::initialize();

    const auto inner = innerWidget();
    const int innerW = inner->width();

    mPopularity = new eOverviewEntry(window());
    mPopularity->setWidth(innerW);
    mPopularity->initialize(Language::zeusText(61, 1)); // popularity
    inner->addWidget(mPopularity);

    mFoodLevel = new eOverviewEntry(window());
    mFoodLevel->setWidth(innerW);
    mFoodLevel->initialize(Language::zeusText(61, 4)); // food level
    inner->addWidget(mFoodLevel);

    mUnemployment = new eOverviewEntry(window());
    mUnemployment->setWidth(innerW);
    mUnemployment->initialize(Language::zeusText(61, 107)); // unemployment
    inner->addWidget(mUnemployment);

    mHygiene = new eOverviewEntry(window());
    mHygiene->setWidth(innerW);
    mHygiene->initialize(Language::zeusText(61, 6)); // hygiene
    inner->addWidget(mHygiene);

    mUnrest = new eOverviewEntry(window());
    mUnrest->setWidth(innerW);
    mUnrest->initialize(Language::zeusText(61, 7)); // unrest
    inner->addWidget(mUnrest);

    mFinances = new eOverviewEntry(window());
    mFinances->setWidth(innerW);
    mFinances->initialize(Language::zeusText(61, 8)); // finances
    inner->addWidget(mFinances);

    const auto spacer1 = new eWidget(window());
    spacer1->setHeight(spacing());
    inner->addWidget(spacer1);

    const auto l1 = new eLineWidget(window());
    l1->setNoPadding();
    l1->fitContent();
    l1->setWidth(innerW);
    inner->addWidget(l1);

    const auto spacer2 = new eWidget(window());
    spacer2->setHeight(spacing());
    inner->addWidget(spacer2);

    const auto requestsLabel = new eLabel(window());
    requestsLabel->setFontSizeXS();
    requestsLabel->setNoPadding();
    requestsLabel->setText(Language::zeusText(61, 195)); // requests
    requestsLabel->fitContent();
    inner->addWidget(requestsLabel);
    requestsLabel->align(Alignment::hcenter);

    mQuestButtons = new eScrollWidget(window());
    mQuestButtons->setWidth(innerW);
    const int requestListH = inner->height() - stackedChildrenHeight(inner);
    mQuestButtons->setHeight(std::max(0, requestListH));
    mQuestButtons->initializeButtons();
    inner->addWidget(mQuestButtons);

    inner->stackVertically();

    updateRequestButtons();
}

void OverviewDataWidget::shown() {
    eDataWidget::show();
    if(mMap) mMap->scheduleUpdate();
}

stdsptr<Texture> sGodIcon(const eUIScale scale,
                           const GodType god) {
    const auto& intrfc = GameTextures::interface();
    (void)scale;
    const int iRes = GameTextures::interfaceTextureId();
    const auto& coll = intrfc[iRes];
    switch(god) {
    case GodType::zeus:
        return coll.fZeusQuestIcon;
    case GodType::poseidon:
        return coll.fPoseidonQuestIcon;
    case GodType::demeter:
        return coll.fDemeterQuestIcon;
    case GodType::apollo:
        return coll.fApolloQuestIcon;
    case GodType::artemis:
        return coll.fArtemisQuestIcon;
    case GodType::ares:
        return coll.fAresQuestIcon;
    case GodType::aphrodite:
        return coll.fAphroditeQuestIcon;
    case GodType::hermes:
        return coll.fHermesQuestIcon;
    case GodType::athena:
        return coll.fAthenaQuestIcon;
    case GodType::hephaestus:
        return coll.fHephaestusQuestIcon;
    case GodType::dionysus:
        return coll.fDionysusQuestIcon;
    case GodType::hades:
        return coll.fHadesQuestIcon;

    case GodType::hera:
        return coll.fHeraQuestIcon;
    case GodType::atlas:
        return coll.fAtlasQuestIcon;
    }
    return nullptr;
}

class eRequestButton : public eButtonBase {
protected:
    using eButtonBase::eButtonBase;

    using eViableChecker = std::function<bool()>;
    using eStatusProvider = std::function<std::string()>;
    using eStatusWarningProvider = std::function<bool()>;
    void initialize(const stdsptr<Texture>& icon,
                    const std::string& txt,
                    const eViableChecker& checker,
                    const eStatusProvider& statusProvider = nullptr,
                    const eStatusWarningProvider& statusWarningProvider = nullptr) {
        setNoPadding();

        mViableChecker = checker;
        mStatusProvider = statusProvider;
        mStatusWarningProvider = statusWarningProvider;

        mStateLabel = new eLabel(window());
        mStateLabel->setNoPadding();
        addWidget(mStateLabel);
        setViable(false);
        mStateLabel->fitContent();

        const auto iconLabel = new eLabel(window());
        iconLabel->setNoPadding();
        iconLabel->setTexture(icon);
        iconLabel->fitContent();
        addWidget(iconLabel);

        const auto textLabel = new eLabel(window());
        textLabel->setFontSizeXS();
        textLabel->setNoPadding();
        textLabel->setText(txt);
        textLabel->fitContent();
        addWidget(textLabel);
        mTextLabel = textLabel;

        if(mStatusProvider) {
            mStatusLabel = new eLabel(window());
            mStatusLabel->setFontSizeXS();
            mStatusLabel->setNoPadding();
            addWidget(mStatusLabel);
            updateStatus();
        }

        setMouseEnterAction([textLabel]() {
            textLabel->setYellowFontColor();
        });
        setMouseLeaveAction([textLabel]() {
            textLabel->setLightFontColor();
        });

        stackHorizontally();
        fitHeight();
        mStateLabel->align(Alignment::vcenter);
        iconLabel->align(Alignment::vcenter);
        textLabel->align(Alignment::vcenter);
        layoutStatus();
    }
protected:
    void paintEvent(ePainter& p) override {
        if(mViableChecker) {
            const bool v = mViableChecker();
            setViable(v);
        }
        updateStatus();
        eButtonBase::paintEvent(p);
    }
private:
    void setViable(const bool f) {
        const auto res = resolution();
        const auto scale = res.uiScale();
        (void)scale;
        const int iRes = GameTextures::interfaceTextureId();
        const auto& intrfc = GameTextures::interface();
        const auto& texs = intrfc[iRes];
        const auto& coll = f ? texs.fRequestFulfilledBox :
                               texs.fRequestWaitingBox;
        const auto tex = coll.getTexture(0);
        mStateLabel->setTexture(tex);
    }

    void updateStatus() {
        if(!mStatusLabel || !mStatusProvider) return;
        const auto text = mStatusProvider();
        if(mStatusLabel->text() != text) {
            mStatusLabel->setText(text);
            mStatusLabel->fitContent();
        }
        const bool warning = mStatusWarningProvider && mStatusWarningProvider();
        if(warning) mStatusLabel->setYellowFontColor();
        else mStatusLabel->setLightFontColor();
        layoutStatus();
    }

    void layoutStatus() {
        if(!mStatusLabel) return;
        mStatusLabel->align(Alignment::right | Alignment::vcenter);
        if(mTextLabel) {
            const int maxW = mStatusLabel->x() - mTextLabel->x();
            if(maxW > 0 && mTextLabel->width() > maxW) {
                mTextLabel->setWidth(maxW);
            }
        }
    }

    eViableChecker mViableChecker;
    eStatusProvider mStatusProvider;
    eStatusWarningProvider mStatusWarningProvider;
    eLabel* mStateLabel = nullptr;
    eLabel* mTextLabel = nullptr;
    eLabel* mStatusLabel = nullptr;
};

class eResourceRequestButton : public eRequestButton {
public:
    using eRequestButton::eRequestButton;

void initialize(const eResourceType resource,
                     const stdsptr<WorldCity>& city,
                     const eViableChecker& checker,
                     const eStatusProvider& statusProvider,
                     const eStatusWarningProvider& statusWarningProvider) {
         const auto cityName = trimmedString(city->name());
         const auto res = resolution();
        const auto uiScale = res.uiScale();
        const auto resIcon = eResourceTypeHelpers::icon(uiScale, resource);

        eRequestButton::initialize(resIcon, cityName, checker,
                                   statusProvider, statusWarningProvider);
    }
};

class eTroopsRequestButton : public eRequestButton {
public:
    using eRequestButton::eRequestButton;

void initialize(const stdsptr<WorldCity>& city,
                     const eViableChecker& checker) {
         const auto cityName = trimmedString(city->name());
         const auto res = resolution();
        const auto uiScale = res.uiScale();
        const int iRes = GameTextures::interfaceTextureId();
        const auto& intrfc = GameTextures::interface();
        const auto& texs = intrfc[iRes];
        const auto& troopsIcon = texs.fTroopsRequestIcon;
        eRequestButton::initialize(troopsIcon, cityName, checker);
    }
};

class eGodQuestButton : public eRequestButton {
public:
    using eRequestButton::eRequestButton;

    void initialize(const GodType god,
                    const eViableChecker& checker) {
        const auto godName = God::sGodName(god);
        const auto res = resolution();
        const auto uiScale = res.uiScale();
        const auto godIcon = sGodIcon(uiScale, god);

        eRequestButton::initialize(godIcon, godName, checker);
    }
};

void OverviewDataWidget::updateRequestButtons() {
    const auto sa = new eWidget(window());
    sa->setNoPadding();
    sa->setWidth(mQuestButtons->width());
    addGodQuests(sa);
    addCityRequests(sa);
    sa->stackVertically();
    sa->fitHeight();
    fitScrollAreaToButtons(mQuestButtons, sa);
    mQuestButtons->setScrollArea(sa);
}

void OverviewDataWidget::setMap(eMiniMap* const map) {
    mMap = map;
}

void OverviewDataWidget::paintEvent(ePainter& p) {
    const bool update = ((mTime++) % 20) == 0;
    if(update) {
        const auto cid = viewedCity();
        {
            const int pop = mBoard.popularity(cid);
            int string = -1;
            if(pop > 90) {
                string = 38; // superb
            } else if(pop > 85) {
                string = 37; // great
            } else if(pop > 80) {
                string = 36; // high
            } else if(pop > 75) {
                string = 34; // good
            } else if(pop > 70) {
                string = 33; // ok
            } else if(pop > 60) {
                string = 32; // poor
            } else if(pop > 50) {
                string = 31; // bad
            } else if(pop > 40) {
                string = 28; // awful
            }else {
                string = 27; // terrible
            }
            mPopularity->setText(Language::zeusText(61, string));
        }
        {
            const auto husbData = mBoard.husbandryData(cid);
            if(husbData) {
                const int a = husbData->canSupport();
                const int pop = mBoard.population(cid);
                int string = -1;
                if(pop == 0 || a < 0.75*pop) {
                    string = 94; // too low
                } else if(a < 0.85*pop) {
                    string = 95; // low
                } else {
                    string = 97; // good
                }
                mFoodLevel->setText(Language::zeusText(61, string));
            }
        }
        {
            const auto emplData = mBoard.employmentData(cid);
            if(emplData) {
                const int f = emplData->freeJobVacancies();
                const int w = emplData->employable();
                const int u = emplData->unemployed();
                if(u == 0) {
                    mUnemployment->setTitle(Language::zeusText(61, 115)); // employment good
                    mUnemployment->setText("");
                } else if(f > 0) {
                    mUnemployment->setTitle(Language::zeusText(61, 111)); // workers needed
                    mUnemployment->setText(std::to_string(f));
                } else {
                    mUnemployment->setTitle(Language::zeusText(61, 107)); // unemployment
                    int per = w == 0 ? 0 : std::round(100.*u/w);
                    per = std::clamp(per, 0, 100);
                    mUnemployment->setText(std::to_string(per) + "%");
                }
            }
        }
        {
            const int hygiene = mBoard.health(cid);
            int string = -1;
            if(hygiene > 90) {
                string = 137; // perfect
            } else if(hygiene > 85) {
                string = 136; // great
            } else if(hygiene > 80) {
                string = 135; // excellent
            } else if(hygiene > 75) {
                string = 134; // very good
            } else if(hygiene > 70) {
                string = 133; // good
            } else if(hygiene > 65) {
                string = 132; // ok
            } else if(hygiene > 60) {
                string = 131; // not good
            } else if(hygiene > 55) {
                string = 130; // poort
            } else if(hygiene > 50) {
                string = 129; // bad
            } else if(hygiene > 45) {
                string = 128; // terrible
            } else {
                string = 127; // appalling
            }
            mHygiene->setText(Language::zeusText(61, string));
        }
        {
            const int unrest = mBoard.unrest(cid);
            int string = -1;
            if(unrest == 0) {
                string = 149; // none
            } else if(unrest > 10) {
                string = 144; // severe
            } else if(unrest > 5) {
                string = 146; // high
            } else {
                string = 148; // low
            }
            mUnrest->setText(Language::zeusText(61, string));
        }
        {
            const auto finances = mBoard.finances(cid);
            const auto& year = finances.thisYear();
            int string;
            if(year.netInOutFlow() > 250) {
                string = 153; // up
            } else if(year.netInOutFlow() < -100) {
                string = 155; // down
            } else {
                string = 154; // ok
            }

            mFinances->setText(Language::zeusText(61, string));
        }
    }
    eWidget::paintEvent(p);
}

bool sHeroReady(GameBoard& board, const eHeroType hero) {
    eHerosHall* hh = nullptr;
    const auto cids = board.personPlayerCitiesOnBoard();
    for(const auto cid : cids) {
        hh = board.heroHall(cid, hero);
        if(hh) break;
    }
    if(!hh) return false;
    const auto s = hh->stage();
    return s == eHeroSummoningStage::arrived;
}

void OverviewDataWidget::addGodQuests(eWidget* const w) {
    const auto pid = mBoard.personPlayer();
    const auto& qs = mBoard.godQuests(pid);
    for(const auto qq : qs) {
        const auto q = qq->godQuest();
        const auto god = q.fGod;
        const auto b = new eGodQuestButton(window());
        b->setWidth(w->width());
        b->initialize(god, [this, q]() {
            return sHeroReady(mBoard, q.fHero);
        });
        b->setPressAction([this, q, qq, pid]() {
            eHerosHall* hh = nullptr;
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            for(const auto cid : cids) {
                hh = mBoard.heroHall(cid, q.fHero);
                if(hh) break;
            }
            const auto heroName = eHero::sHeroName(q.fHero);
            const auto gw = gameWidget();
            std::string heroNeededTmpl;
            StringHelpers::replace(heroNeededTmpl, "[hero_name]", heroName);
            if(hh) {
                const auto s = hh->stage();
                if(s == eHeroSummoningStage::arrived) {
                    const auto acceptA = [qq]() {
                        qq->fulfill();
                    };
                    const auto title = Language::zeusText(185, 121);
                    auto text = Language::zeusText(185, 122);
                    StringHelpers::replace(text, "[hero_name]", heroName);
                    const auto questName = q.name();
                    StringHelpers::replace(text, "[god_quest]", questName);
                    gw->showQuestion(title, text, acceptA);
                } else {
                    gw->showTip(pid, heroNeededTmpl);
                }
            } else {
                gw->showTip(pid, heroNeededTmpl);
            }
        });
        w->addWidget(b);
    }
}

void OverviewDataWidget::addCityRequests(eWidget* const w) {
    const auto pid = mBoard.personPlayer();
    const auto& qs = mBoard.cityRequests(pid);
    const auto& tqs = mBoard.tributeRequests(pid);
    for(const auto& qq : qs) {
        const auto q = qq->cityRequest();
        const auto b = new eResourceRequestButton(window());
        b->setWidth(w->width());
        b->initialize(q.fType, q.fCity, [this, q]() {
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            for(const auto cid : cids) {
                const auto count = mBoard.resourceCount(cid, q.fType);
                if(count >= q.fCount) return true;
            }
            return false;
        }, [this, qq]() {
            return qq->overdueStatusText(mBoard.date());
        }, [this, qq]() {
            return qq->isPostponed();
        });
        b->setPressAction([this, q, qq, pid]() {
            const auto gw = gameWidget();
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            for(const auto cid : cids) {
                const auto count = mBoard.resourceCount(cid, q.fType);
                if(count >= q.fCount) {
                    const auto acceptA = [qq, cid]() {
                        qq->dispatch(cid);
                    };
                    const auto title = Language::zeusText(5, 6); // Request
                    const auto text = qq->dispatchText(count, mBoard.date());
                    gw->showQuestion(title, text, q.fType, acceptA);
                } else {
                    const auto tip = Language::zeusText(5, 9); // You do not have enough to fulfill the request
                    gw->showTip(pid, tip);
                }
            }
        });
        w->addWidget(b);
    }
    for(const auto& qq : tqs) {
        const auto q = qq->cityRequest();
        const auto b = new eResourceRequestButton(window());
        b->setWidth(w->width());
        b->initialize(q.fType, q.fCity, [this, q]() {
            if(q.fType == eResourceType::drachmas) {
                return mBoard.drachmas(mBoard.personPlayer()) >= q.fCount;
            }
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            for(const auto cid : cids) {
                const auto count = mBoard.resourceCount(cid, q.fType);
                if(count >= q.fCount) return true;
            }
            return false;
        }, [this, qq]() {
            return qq->overdueStatusText(mBoard.date());
        }, [qq]() {
            return qq->isPostponed();
        });
        b->setPressAction([this, q, qq, pid]() {
            const auto gw = gameWidget();
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            if(q.fType == eResourceType::drachmas) {
                const auto count = mBoard.drachmas(mBoard.personPlayer());
                if(count >= q.fCount && !cids.empty()) {
                    const auto cid = cids[0];
                    const auto acceptA = [qq, cid]() {
                        qq->dispatch(cid);
                    };
                    const auto title = Language::zeusText(5, 6); // Request
                    const auto text = qq->dispatchText(count, mBoard.date());
                    gw->showQuestion(title, text, q.fType, acceptA);
                } else {
                    const auto tip = Language::zeusText(5, 9); // You do not have enough to fulfill the request
                    gw->showTip(pid, tip);
                }
                return;
            }
            for(const auto cid : cids) {
                const auto count = mBoard.resourceCount(cid, q.fType);
                if(count >= q.fCount) {
                    const auto acceptA = [qq, cid]() {
                        qq->dispatch(cid);
                    };
                    const auto title = Language::zeusText(5, 6); // Request
                    const auto text = qq->dispatchText(count, mBoard.date());
                    gw->showQuestion(title, text, q.fType, acceptA);
                } else {
                    const auto tip = Language::zeusText(5, 9); // You do not have enough to fulfill the request
                    gw->showTip(pid, tip);
                }
            }
        });
        w->addWidget(b);
    }
    const auto& qqs = mBoard.cityTroopsRequests(pid);
    for(const auto& qq : qqs) {
        const auto b = new eTroopsRequestButton(window());
        b->setWidth(w->width());
        b->initialize(qq->city(), [this]() {
            const auto cids = mBoard.personPlayerCitiesOnBoard();
            for(const auto cid : cids) {
                const auto& bs = mBoard.banners(cid);
                for(const auto& b : bs) {
                    const bool a = b->isAbroad();
                    if(!a) return true;
                }
                const auto hs = mBoard.heroHalls(cid);
                for(const auto h : hs) {
                    const bool a = h->heroOnQuest();
                    if(!a) return true;
                }
            }
            return false;
        });
        b->setPressAction([qq]() {
            qq->dispatch();
        });
        w->addWidget(b);
    }
}
