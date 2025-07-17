#include "esanctuaryinfowidget.h"

#include "elanguage.h"
#include "widgets/eframedbutton.h"
#include "characters/gods/egod.h"
#include "estringhelpers.h"
#include "buildings/sanctuaries/esanctuary.h"
#include "engine/egameboard.h"
#include "widgets/echoosecitydialog.h"
#include "evectorhelpers.h"
#include "widgets/egamewidget.h"

eSanctuaryInfoWidget::eSanctuaryInfoWidget(
        eMainWindow* const window,
        eMainWidget* const mw) :
    eEmployingBuildingInfoWidget(window, mw, false, false) {}

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

void eSanctuaryInfoWidget::initialize(eSanctuary* const s) {
    if(s->finished()) {
        const auto title = eBuilding::sNameForBuilding(s);
        eInfoWidget::initialize(title);
        std::string employmentInfo;
        {
            std::string title;
            std::string info;
            std::string additionalInfo;
            eBuilding::sInfoText(s, title, info,
                                 employmentInfo,
                                 additionalInfo);
        }
        addText(employmentInfo);
        const auto cw = addCentralWidget();
        addEmploymentWidget(s);
        const auto gt = s->godType();

        const int cww = cw->width();
        const auto descLabel = new eLabel(window());
        descLabel->setNoPadding();
        descLabel->setSmallFontSize();
        descLabel->setWrapWidth(cww);
        const int godId = sTextGodId(gt);
        std::string desc;
        {
            const int string = 66 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        {
            const int string = 80 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        {
            const int string = 94 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            desc = desc + " " + txt;
        }
        descLabel->setText(desc);
        descLabel->fitContent();
        cw->addWidget(descLabel);

        const auto buttonReasonW = new eWidget(window());
        buttonReasonW->setNoPadding();
        cw->addWidget(buttonReasonW);
        buttonReasonW->setWidth(cww);

        const auto reasonLabel = new eLabel(window());
        reasonLabel->setNoPadding();
        reasonLabel->setSmallFontSize();
        reasonLabel->setWrapWidth(cww);
        buttonReasonW->addWidget(reasonLabel);

        const auto buttonsW = new eWidget(window());
        buttonsW->setNoPadding();

        {
            const int string = 10 + godId;
            const auto txt = eLanguage::zeusText(132, string);
            const auto pb = new eFramedButton(txt, window());
            pb->setUnderline(false);
            pb->fitContent();
            buttonsW->addWidget(pb);
            pb->align(eAlignment::hcenter);
            pb->setPressAction([s, godId, buttonReasonW, reasonLabel]() {
                eHelpDenialReason reason;
                const bool r = s->askForHelp(reason);
                int string;
                if(!r) {
                    switch(reason) {
                    case eHelpDenialReason::tooSoon:
                        string = 52 + godId;
                        break;
                    case eHelpDenialReason::noTarget:
                        string = 38 + godId;
                        break;
                    case eHelpDenialReason::error:
                        string = -1;
                        break;
                    }
                    reasonLabel->setYellowFontColor();
                } else {
                    string = 24 + godId;
                    reasonLabel->setLightFontColor();
                }
                const auto txt = eLanguage::zeusText(132, string);
                reasonLabel->setText(txt);
                reasonLabel->fitContent();

                buttonReasonW->stackVertically();
                buttonReasonW->fitHeight();
                buttonReasonW->align(eAlignment::bottom);
            });
        }
        const auto& board = s->getBoard();
        const auto cids = board.citiesOnBoard();
        const auto ppid = board.personPlayer();
        const auto ptid = board.playerIdToTeamId(ppid);
        std::vector<eCityId> enemyCids;
        for(const auto cid : cids) {
            const auto ctid = board.cityIdToTeamId(cid);
            if(ctid == eTeamId::neutralFriendly) continue;
            if(ctid == eTeamId::neutralAggresive) continue;
            if(ctid == ptid) continue;
            enemyCids.push_back(cid);
        }
        if(!enemyCids.empty()) {
            const auto txt = eLanguage::zeusText(156, 27);
            const auto pb = new eFramedButton(txt, window());
            pb->setUnderline(false);
            pb->fitContent();
            buttonsW->addWidget(pb);
            pb->align(eAlignment::hcenter);
            const auto wboard = board.getWorldBoard();
            pb->setPressAction([this, wboard, s, buttonReasonW, reasonLabel, enemyCids]() {
                const auto askForAttack = [s, buttonReasonW, reasonLabel](const eCityId cid) {
                    eHelpDenialReason reason;
                    const bool r = s->askForAttack(cid, reason);
                    int string;
                    if(!r) {
                        switch(reason) {
                        case eHelpDenialReason::tooSoon:
                            string = 19 + (eRand::rand() % 6);
                            break;
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

                    buttonReasonW->stackVertically();
                    buttonReasonW->fitHeight();
                    buttonReasonW->align(eAlignment::bottom);
                };
                if(enemyCids.size() == 1) {
                    askForAttack(enemyCids[0]);
                } else {
                    const auto choose = new eChooseCityDialog(window());
                    choose->setValidator([enemyCids](const stdsptr<eWorldCity>& c) {
                        const auto cid = c->cityId();
                        return eVectorHelpers::contains(enemyCids, cid);
                    });
                    const auto act = [askForAttack](const stdsptr<eWorldCity>& c) {
                        const auto cid = c->cityId();
                        askForAttack(cid);
                    };
                    choose->initialize(wboard, act);

                    const auto mw = mainWidget();
                    mw->openDialog(choose);
                }
            });
        }

        const int p = resolution().largePadding();
        buttonsW->stackHorizontally(p);
        buttonsW->fitContent();

        buttonReasonW->addWidget(buttonsW);
        buttonsW->align(eAlignment::hcenter);

        buttonReasonW->stackVertically();
        buttonReasonW->fitHeight();
        buttonReasonW->align(eAlignment::bottom);
    } else {
        const auto name = eBuilding::sNameForBuilding(s);
        auto title = eLanguage::zeusText(178, 2);
        eStringHelpers::replace(title, "[monument]", name);
        eInfoWidget::initialize(title);

        const bool r = s->accessToRoad();
        if(!r) {
            addText(eLanguage::zeusText(69, 4));
        }
        const bool h = s->constructionHalted();
        if(h) {
            addText(eLanguage::zeusText(132, 130));
        }
        auto& board = s->getBoard();
        const auto cid = s->cityId();
        const int na = board.countBuildings(cid, eBuildingType::artisansGuild);
        if(na == 0) {
            addText(eLanguage::zeusText(178, 0));
        }

        const int p = s->progress();
        const auto pStr = std::to_string(p);
        const auto god = s->godType();
        const auto godStr = eGod::sGodName(god);
        auto complete = eLanguage::zeusText(178, 23);
        eStringHelpers::replace(complete, "[god]", godStr);
        eStringHelpers::replace(complete, "[percent_complete]", pStr + "%");
        addText(complete);

        const auto cost = s->cost();
        const auto stored = s->stored();
        const auto used = s->used();
        const auto needed = cost - stored - used;
        const int nm = needed.fMarble;
        const auto nmStr = std::to_string(nm);
        const int nw = needed.fWood;
        const auto nwStr = std::to_string(nw);
        const int ns = needed.fSculpture;
        const auto nsStr = std::to_string(ns);
        if(nm > 0 || nw > 0 || ns > 0) {
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
            addText(rem);
        } else {
            const auto all = eLanguage::zeusText(178, 36);
            addText(all);
        }

        const auto cw = addCentralWidget();
        const auto haltB = new eFramedButton(window());
        haltB->setUnderline(false);
        haltB->setText(h ? eLanguage::zeusText(132, 113) :
                           eLanguage::zeusText(132, 112));
        haltB->fitContent();
        haltB->setPressAction([s, haltB]() {
            bool h = s->constructionHalted();
            h = !h;
            s->setConstructionHalted(h);
            haltB->setText(h ? eLanguage::zeusText(132, 113) :
                               eLanguage::zeusText(132, 112));
            haltB->fitContent();
            haltB->align(eAlignment::hcenter);
        });
        cw->addWidget(haltB);
        haltB->align(eAlignment::hcenter | eAlignment::bottom);
    }
}
