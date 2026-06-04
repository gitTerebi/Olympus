#include "esanctuaryinfowidget.h"

#include "elanguage.h"
#include "widgets/framed-button.h"
#include "characters/gods/egod.h"
#include "estringhelpers.h"
#include "buildings/sanctuaries/sanctuary.h"
#include "buildings/pyramids/epyramid.h"
#include "engine/game-board.h"
#include "widgets/echoosecitydialog.h"
#include "evectorhelpers.h"
#include "widgets/game-widget.h"
#include "widgets/eprogressbar.h"
#include "widgets/elayouthelpers.h"

eSanctuaryInfoWidget::eSanctuaryInfoWidget(
        eMainWindow* const window,
        eMainWidget* const mw) :
    eEmployingBuildingInfoWidget(window, mw, true, false) {}

int sTextGodId(const eGodType god) {
    switch(god) {
    case eGodType::zeus:
        return 0;
    case eGodType::poseidon:
        return 1;
    case eGodType::demeter:
        return 2;
    case eGodType::apollo:
        return 3;
    case eGodType::artemis:
        return 4;
    case eGodType::ares:
        return 5;
    case eGodType::aphrodite:
        return 6;
    case eGodType::hermes:
        return 7;
    case eGodType::athena:
        return 8;
    case eGodType::hephaestus:
        return 9;
    case eGodType::dionysus:
        return 10;
    case eGodType::hades:
        return 11;
    case eGodType::hera:
        return 12;
    case eGodType::atlas:
        return 13;
    }
    return 0;
}

void eSanctuaryInfoWidget::initialize(eMonument* const m) {
    assert(m);
    const int p = resolution().paddingL();
    eSanctuary* const s = dynamic_cast<eSanctuary*>(m);
    if(s && s->finished()) {
        const auto title = eBuilding::sNameForBuilding(m);
        eInfoWidget::initialize(title);
        std::string employmentInfo;
        {
            std::string title;
            std::string info;
            std::string additionalInfo;
            eBuilding::sInfoText(m, title, info,
                                 employmentInfo,
                                 additionalInfo);
        }
        // override with live priest state (sInfoText only knows employment level)
        if(s->priestOut()) {
            employmentInfo = eLanguage::zeusText(132, 6); // "Our priests are out looking..."
        } else if(s->sacrificing()) {
            employmentInfo = eLanguage::zeusText(132, 7); // "We are conducting sacrifices..."
        }
        const auto cw = addCentralWidget();
        addEmploymentWidget(m, employmentInfo);

        {
            const auto makeLbl = [this](const std::string& txt) {
                const auto lbl = new eLabel(window());
                lbl->setFontSizeS();
                lbl->setText(txt);
                lbl->fitContent();
                return lbl;
            };
            std::vector<eLayoutHelpers::eFlexItem> lines;
            if(s->sacrificing()) {
                const int days = s->sacrificeDaysLeft();
                if(days > 0) {
                    // "This Sanctuary is active for another X days"
                    const auto txt = eLanguage::zeusText(132, 8) +
                                     " " + std::to_string(days) + " days";
                    lines.push_back({makeLbl(txt)});
                } else {
                    // "We are conducting sacrifices right now."
                    lines.push_back({makeLbl(eLanguage::zeusText(132, 7))});
                }
            } else if(s->priestOut()) {
                // "Sanctuary preparing for sacrifice"
                lines.push_back({makeLbl(eLanguage::zeusText(59, 30))});
            } else {
                // "Sanctuary working normally"
                lines.push_back({makeLbl(eLanguage::zeusText(59, 26))});
            }
            const auto col = eLayoutHelpers::flexCol(
                                 window(), widgetWidth(), 0, lines,
                                 {.align = eLayoutHelpers::eAlign::center});
            addInfoWidget(col);
        }

        const auto gt = s->godType();

        const int cww = cw->width();
        const int godId = sTextGodId(gt);

        const auto descLabel = new eLabel(window());
        descLabel->setNoPadding();
        descLabel->setFontSizeS();
        descLabel->setWrapWidth(cww);
        std::string desc;
        desc += eLanguage::zeusText(132, 66 + godId);
        desc += " " + eLanguage::zeusText(132, 80 + godId);
        desc += " " + eLanguage::zeusText(132, 94 + godId);
        descLabel->setText(desc);
        descLabel->fitContent();
        descLabel->setWidth(cww);

        const auto buttonReasonW = new eWidget(window());
        buttonReasonW->setNoPadding();
        buttonReasonW->setWidth(cww);

        const auto reasonLabel = new eLabel(window());
        reasonLabel->setNoPadding();
        reasonLabel->setFontSizeS();
        reasonLabel->setWrapWidth(cww);
        {
            // initial status line
            const bool aresGod = (gt == eGodType::ares);
            if(aresGod && s->aresBuffReady()) {
                reasonLabel->setLightFontColor();
                reasonLabel->setText(eLanguage::zeusText(132, 38 + godId)); // "Ares has heard...next opportunity"
            } else if(s->prayerReady()) {
                reasonLabel->setLightFontColor();
                reasonLabel->setText(eLanguage::zeusText(132, 94 + godId));
            } else if(aresGod) {
                reasonLabel->setYellowFontColor();
                reasonLabel->setText(eLanguage::zeusText(132, 99)); // "Pray to Ares if you would like him to accompany..."
            } else {
                reasonLabel->setYellowFontColor();
                reasonLabel->setText(eLanguage::zeusText(132, 52 + godId));
            }
            reasonLabel->fitContent();
        }
        buttonReasonW->addWidget(reasonLabel);

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();

        {
            const auto bw = new eWidget(window());
            bw->setNoPadding();
            buttonsW->addWidget(bw);
            const auto pb = new FramedButton(eLanguage::zeusText(132, 10 + godId), window());
            pb->setUnderline(false);
            pb->fitContent();
            if(gt == eGodType::ares && s->aresBuffReady()) pb->setEnabled(false);
            bw->addWidget(pb);
            const auto bar = new eProgressBar(window());
            bar->setRange(0, 100);
            bar->setValue(std::clamp(int(std::floor(100*s->helpTimeFraction())), 0, 100));
            pb->setPressAction([s, godId, gt, pb, buttonReasonW, reasonLabel, bar, cww, p]() {
                eHelpDenialReason reason;
                const bool r = s->askForHelp(reason);
                reasonLabel->setWrapWidth(cww);
                reasonLabel->setWidth(cww);
                if(!r) {
                    int string;
                    switch(reason) {
                    case eHelpDenialReason::tooSoon:
                        string = 52 + godId;
                        break;
                    case eHelpDenialReason::noTarget:
                        string = 38 + godId;
                        break;
                    default:
                        string = 52 + godId;
                        break;
                    }
                    reasonLabel->setYellowFontColor();
                    reasonLabel->setText(eLanguage::zeusText(132, string));
                } else if(gt == eGodType::ares) {
                    reasonLabel->setLightFontColor();
                    reasonLabel->setText(eLanguage::zeusText(132, 38 + godId));
                    pb->setEnabled(false);
                } else {
                    reasonLabel->setLightFontColor();
                    reasonLabel->setText(eLanguage::zeusText(132, 24 + godId));
                }
                reasonLabel->fitContent();
                bar->setValue(std::clamp(int(std::floor(100*s->helpTimeFraction())), 0, 100));
                buttonReasonW->stackVertically(p);
                buttonReasonW->fitHeight();
            });
            bw->addWidget(bar);
            bar->resize(pb->width(), p);
            bw->stackVertically(p);
            bw->fitContent();
        }
        const auto& board = m->getBoard();
        const auto cids = board.citiesOnBoard();
        const auto pid = m->playerId();
        const auto ptid = board.playerIdToTeamId(pid);
        const auto enemyCids = board.enemyCidsOnBoard(ptid);
        if(!enemyCids.empty()) {
            const auto bw = new eWidget(window());
            bw->setNoPadding();
            buttonsW->addWidget(bw);
            const auto txt = eLanguage::zeusText(156, 27);
            const auto pb = new FramedButton(txt, window());
            pb->setUnderline(false);
            pb->fitContent();
            bw->addWidget(pb);
            const auto wboard = &board.world();
            const auto bar = new eProgressBar(window());
            bar->setRange(0, 100);
            const double frac = s->helpAttackTimeFraction();
            bar->setValue(std::clamp(int(std::floor(100*frac)), 0, 100));
            pb->setPressAction([this, wboard, s, buttonReasonW, reasonLabel, enemyCids, bar]() {
                const auto askForAttack = [s, buttonReasonW, reasonLabel, bar](const eCityId cid) {
                    eHelpDenialReason reason;
                    const bool r = s->askForAttack(cid, reason);
                    int string;
                    if(!r) {
                        switch(reason) {
                        case eHelpDenialReason::tooSoon: {
                            const double frac = s->helpAttackTimeFraction();
                            const int per = std::clamp(int(std::floor(100*frac)), 0, 100);
                            string = 19 + per/17;
                        } break;
                        case eHelpDenialReason::noTarget:
                        case eHelpDenialReason::error:
                            string = -1;
                            break;
                        }
                        reasonLabel->setYellowFontColor();
                    } else {
                        string = 25;
                        reasonLabel->setLightFontColor();
                    }
                    const auto godType = s->godType();
                    const auto godName = eGod::sGodName(godType);
                    const auto txt = godName + " " + eLanguage::zeusText(59, string);
                    reasonLabel->setText(txt);
                    reasonLabel->fitContent();
                    const double f = s->helpAttackTimeFraction();
                    bar->setValue(std::clamp(int(std::floor(100*f)), 0, 100));
                };
                if(enemyCids.size() == 1) {
                    askForAttack(enemyCids[0]);
                } else {
                    const auto choose = new eChooseCityDialog(window());
                    choose->setValidator([enemyCids](const stdsptr<WorldCity>& c) {
                        const auto cid = c->cityId();
                        return eVectorHelpers::contains(enemyCids, cid);
                    });
                    const auto act = [askForAttack](const stdsptr<WorldCity>& c) {
                        const auto cid = c->cityId();
                        askForAttack(cid);
                    };
                    choose->initialize(wboard, act);

                    const auto mw = mainWidget();
                    mw->openDialog(choose);
                }
            });
            bw->addWidget(bar);
            const int w = pb->width();
            bar->resize(w, p);

            bw->stackVertically(p);
            bw->fitContent();
        }

        buttonsW->stackHorizontally(p);
        buttonsW->fitContent();

        buttonReasonW->addWidget(buttonsW);
        buttonsW->align(eAlignment::hcenter);
        buttonReasonW->stackVertically(p);
        buttonReasonW->fitHeight();

        // stack desc + buttonReasonW into cw with spacing
        cw->addWidget(descLabel);
        cw->addWidget(buttonReasonW);
        descLabel->move(0, 0);
        buttonReasonW->move(0, descLabel->height() + 2*p);
    } else if(m->finished()) {
        const auto title = eBuilding::sNameForBuilding(m);
        eInfoWidget::initialize(title);
        const auto type = m->type();
        int string = -1;
        switch(type) {
        case eBuildingType::modestPyramid:
            string = 114;
            break;
        case eBuildingType::pyramid:
            string = 115;
            break;
        case eBuildingType::greatPyramid:
            string = 116;
            break;
        case eBuildingType::majesticPyramid:
            string = 117;
            break;

        case eBuildingType::smallMonumentToTheSky:
            string = 118;
            break;
        case eBuildingType::monumentToTheSky:
            string = 119;
            break;
        case eBuildingType::grandMonumentToTheSky:
            string = 120;
            break;

        case eBuildingType::minorShrineAphrodite:
        case eBuildingType::minorShrineApollo:
        case eBuildingType::minorShrineAres:
        case eBuildingType::minorShrineArtemis:
        case eBuildingType::minorShrineAthena:
        case eBuildingType::minorShrineAtlas:
        case eBuildingType::minorShrineDemeter:
        case eBuildingType::minorShrineDionysus:
        case eBuildingType::minorShrineHades:
        case eBuildingType::minorShrineHephaestus:
        case eBuildingType::minorShrineHera:
        case eBuildingType::minorShrineHermes:
        case eBuildingType::minorShrinePoseidon:
        case eBuildingType::minorShrineZeus:
            string = 121;
            break;
        case eBuildingType::shrineAphrodite:
        case eBuildingType::shrineApollo:
        case eBuildingType::shrineAres:
        case eBuildingType::shrineArtemis:
        case eBuildingType::shrineAthena:
        case eBuildingType::shrineAtlas:
        case eBuildingType::shrineDemeter:
        case eBuildingType::shrineDionysus:
        case eBuildingType::shrineHades:
        case eBuildingType::shrineHephaestus:
        case eBuildingType::shrineHera:
        case eBuildingType::shrineHermes:
        case eBuildingType::shrinePoseidon:
        case eBuildingType::shrineZeus:
            string = 122;
            break;
        case eBuildingType::majorShrineAphrodite:
        case eBuildingType::majorShrineApollo:
        case eBuildingType::majorShrineAres:
        case eBuildingType::majorShrineArtemis:
        case eBuildingType::majorShrineAthena:
        case eBuildingType::majorShrineAtlas:
        case eBuildingType::majorShrineDemeter:
        case eBuildingType::majorShrineDionysus:
        case eBuildingType::majorShrineHades:
        case eBuildingType::majorShrineHephaestus:
        case eBuildingType::majorShrineHera:
        case eBuildingType::majorShrineHermes:
        case eBuildingType::majorShrinePoseidon:
        case eBuildingType::majorShrineZeus:
            string = 123;
            break;

        case eBuildingType::pyramidOfThePantheon:
            string = 124;
            break;
        case eBuildingType::altarOfOlympus:
            string = 125;
            break;
        case eBuildingType::templeOfOlympus:
            string = 126;
            break;
        case eBuildingType::observatoryKosmika:
            string = 127;
            break;
        case eBuildingType::museumAtlantika:
            string = 128;
            break;
        default:
            break;
        }

        auto text = eLanguage::zeusText(132, string);
        const auto god = ePyramid::sGod(type);
        const auto name = eGod::sGodName(god);
        eStringHelpers::replace(text, "[god]", name);
        eStringHelpers::replace(text, "[god]", name);
        addText(text);
    } else {
        const auto name = eBuilding::sNameForBuilding(m);
        auto title = eLanguage::zeusText(178, 2);
        eStringHelpers::replace(title, "[monument]", name);
        eInfoWidget::initialize(title);

        const bool r = m->accessToRoad();
        if(!r) {
            addText(eLanguage::zeusText(69, 4));
        }
        const bool h = m->constructionHalted();
        if(h) {
            addText(eLanguage::zeusText(132, 130));
        }
        auto& board = m->getBoard();
        const auto cid = m->cityId();
        const int na = board.countBuildings(cid, eBuildingType::artisansGuild);
        if(na == 0) {
            addText(eLanguage::zeusText(178, 0));
        }

        const int p = m->progress();
        const auto pStr = std::to_string(p);
        if(s) {
            const auto god = s->godType();
            const auto godStr = eGod::sGodName(god);
            auto complete = eLanguage::zeusText(178, 23);
            eStringHelpers::replace(complete, "[god]", godStr);
            eStringHelpers::replace(complete, "[percent_complete]", pStr + "%");
            addText(complete);
        } else {
            auto complete = name + " " + eLanguage::zeusText(178, 24);
            eStringHelpers::replace(complete, "[percent_complete]", pStr + "%");
            addText(complete);
        }

        const auto cost = m->cost();
        const auto stored = m->stored();
        const auto used = m->used();
        const auto needed = cost - stored - used;
        const int nm = needed.fMarble;
        const auto nmStr = std::to_string(nm);
        const int nw = needed.fWood;
        const auto nwStr = std::to_string(nw);
        const int ns = needed.fSculpture;
        const auto nsStr = std::to_string(ns);
        const int no = needed.fOrichalc;
        const auto noStr = std::to_string(no);
        const int nbm = needed.fBlackMarble;
        const auto nbmStr = std::to_string(nbm);
        if(nm > 0 || nw > 0 || ns > 0 || no > 0 || nbm > 0) {
            auto rem = eLanguage::zeusText(178, 25);
            if(nm == 1) {
                auto remM = eLanguage::zeusText(178, 26);
                eStringHelpers::replace(remM, "[amount]", nmStr);
                rem += "\n" + remM;
            } else if(nm > 1) {
                auto remM = eLanguage::zeusText(178, 27);
                eStringHelpers::replace(remM, "[amount]", nmStr);
                rem += "\n" + remM;
            }
            if(nw == 1) {
                auto remW = eLanguage::zeusText(178, 30);
                eStringHelpers::replace(remW, "[amount]", nwStr);
                rem += "\n" + remW;
            } else if(nw > 1) {
                auto remW = eLanguage::zeusText(178, 31);
                eStringHelpers::replace(remW, "[amount]", nwStr);
                rem += "\n" + remW;
            }
            if(ns == 1) {
                auto remS = eLanguage::zeusText(178, 32);
                eStringHelpers::replace(remS, "[amount]", nsStr);
                rem += "\n" + remS;
            } else if(ns > 1) {
                auto remS = eLanguage::zeusText(178, 33);
                eStringHelpers::replace(remS, "[amount]", nsStr);
                rem += "\n" + remS;
            }
            if(no == 1) {
                auto remO = eLanguage::zeusText(178, 34);
                eStringHelpers::replace(remO, "[amount]", noStr);
                rem += "\n" + remO;
            } else if(no > 1) {
                auto remO = eLanguage::zeusText(178, 35);
                eStringHelpers::replace(remO, "[amount]", noStr);
                rem += "\n" + remO;
            }
            if(nbm == 1) {
                auto remNbm = eLanguage::zeusText(178, 28);
                eStringHelpers::replace(remNbm, "[amount]", nbmStr);
                rem += "\n" + remNbm;
            } else if(nbm > 1) {
                auto remNbm = eLanguage::zeusText(178, 29);
                eStringHelpers::replace(remNbm, "[amount]", nbmStr);
                rem += "\n" + remNbm;
            }
            addText(rem);
        } else {
            const auto all = eLanguage::zeusText(178, 36);
            addText(all);
        }

        const auto cw = addCentralWidget();
        const auto haltB = new FramedButton(window());
        haltB->setUnderline(false);
        haltB->setText(h ? eLanguage::zeusText(132, 113) :
                           eLanguage::zeusText(132, 112));
        haltB->fitContent();
        haltB->setPressAction([m, haltB]() {
            bool h = m->constructionHalted();
            h = !h;
            m->setConstructionHalted(h);
            haltB->setText(h ? eLanguage::zeusText(132, 113) :
                               eLanguage::zeusText(132, 112));
            haltB->fitContent();
            haltB->align(eAlignment::hcenter);
        });
        cw->addWidget(haltB);
        haltB->align(eAlignment::hcenter | eAlignment::bottom);
    }
}
