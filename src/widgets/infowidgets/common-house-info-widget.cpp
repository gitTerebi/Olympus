#include "common-house-info-widget.h"

#include "buildings/small-house.h"
#include "buildings/elite-housing.h"
#include "engine/eresourcetype.h"
#include "engine/game-board.h"
#include "engine/board-city.h"
#include "engine/difficulty.h"

#include "widgets/elabel.h"

#include "elanguage.h"

#include <algorithm>

CommonHouseInfoWidget::CommonHouseInfoWidget(
        eMainWindow* const window,
        eMainWidget* const mw) :
    eInfoWidget(window, mw, true, true) {}

void CommonHouseInfoWidget::initialize(eHouseBase* const house) {
    const int people = house->people();
    const int level = house->level();
    const auto type = house->type();
    int titleGroup = 29;
    int titleString = 0;
    if(type == eBuildingType::commonHouse) {
        if(people <= 0) {
            titleGroup = 128;
            titleString = 0;
        } else if(level == 0) {
            titleString = 0;
        } else if(level == 1) {
            titleString = 1;
        } else if(level == 2) {
            titleString = 2;
        } else if(level == 3) {
            titleString = 3;
        } else if(level == 4) {
            titleString = 4;
        } else if(level == 5) {
            titleString = 5;
        } else if(level == 6) {
            titleString = 6;
        }
    } else { // elite
        if(people <= 0) {
            titleString = 8;
        } else if(level == 0) {
            titleString = 9;
        } else if(level == 1) {
            titleString = 10;
        } else if(level == 2) {
            titleString = 11;
        } else if(level == 3) {
            titleString = 12;
        }
    }
    const auto title = eLanguage::zeusText(titleGroup, titleString);
    eInfoWidget::initialize(title);
    addCentralWidget();

    const int p = padding();
    const auto fw = addFramedWidget(16*p);

    if(people <= 0) return;

    const auto miss = house->missing();
    int needString;
    switch(miss) {
    case eHouseMissing::water:
        needString = 49;
        break;
    case eHouseMissing::food:
        needString = 51;
        break;
    case eHouseMissing::fleece:
        needString = 54;
        break;
    case eHouseMissing::oil:
        needString = 55;
        break;
    case eHouseMissing::venues:
        needString = 50;
        break;
    case eHouseMissing::appeal:
        needString = 48;
        break;
    case eHouseMissing::wine:
        needString = 53;
        break;
    case eHouseMissing::arms:
        needString = 57;
        break;
    case eHouseMissing::horse:
        needString = 56;
        break;
    case eHouseMissing::nothing:
        needString = 46;
        break;
    }
    std::string msg;
    const auto sh = dynamic_cast<SmallHouse*>(house);
    if(sh && miss != eHouseMissing::nothing &&
       (sh->devolveDelay() > 0 || sh->evictDelay() > 0)) {
        msg = "This house is devolving. It needs ";
        switch(miss) {
        case eHouseMissing::food:
            msg += "food.";
            break;
        case eHouseMissing::water:
            msg += "water.";
            break;
        case eHouseMissing::fleece:
            msg += "fleece.";
            break;
        case eHouseMissing::oil:
            msg += "olive oil.";
            break;
        case eHouseMissing::venues:
            msg += "venues.";
            break;
        case eHouseMissing::appeal:
            msg += "appeal.";
            break;
        case eHouseMissing::wine:
            msg += "wine.";
            break;
        case eHouseMissing::arms:
            msg += "armor.";
            break;
        case eHouseMissing::horse:
            msg += "horses.";
            break;
        case eHouseMissing::nothing:
            msg += "nothing.";
            break;
        }
    } else {
        msg = eLanguage::zeusText(127, needString);
    }

    const auto cw = centralWidget();
    const auto msgLabel = new eLabel(window());
    msgLabel->setFontSizeS();
    msgLabel->setWidth(cw->width() - 2*p);
    msgLabel->setWrapWidth(msgLabel->width());
    msgLabel->setText(msg);
    msgLabel->fitContent();
    cw->addWidget(msgLabel);

    const auto supplies = new eWidget(window());
    supplies->setNoPadding();
    const auto generateSupply = [&](const eResourceType type,
                                    const int count) {
        const auto w = new eWidget(window());
        w->setNoPadding();

        const auto res = resolution();
        const auto tex = eResourceTypeHelpers::icon(res.uiScale(), type);
        const auto iconLabel = new eLabel(window());
        iconLabel->setTexture(tex);
        iconLabel->fitContent();
        w->addWidget(iconLabel);

        const auto countLabel = new eLabel(window());
        countLabel->setFontSizeS();
        const auto cStr = std::to_string(count);
        countLabel->setText(cStr);
        countLabel->fitContent();
        w->addWidget(countLabel);

        w->stackHorizontally();
        w->fitContent();
        iconLabel->align(eAlignment::vcenter);
        countLabel->align(eAlignment::vcenter);

        return w;
    };

    const auto generateSupplyWidget = [&](const std::vector<eWidget*>& wids) {
        const auto w = new eWidget(window());
        w->setNoPadding();

        for(const auto wid : wids) {
            w->addWidget(wid);
        }

        w->setWidth(cw->width());
        w->layoutHorizontally();
        w->fitContent();
        cw->addWidget(w);
        return w;
    };

    {
        const auto food = generateSupply(eResourceType::food, house->food());
        const auto fleece = generateSupply(eResourceType::fleece, house->fleece());
        const auto oil = generateSupply(eResourceType::oliveOil, house->oil());

        const auto w = generateSupplyWidget({food, fleece, oil});
        w->setY(msgLabel->y() + msgLabel->height() + p);

        if(house->type() == eBuildingType::eliteHousing) {
            const auto eh = static_cast<EliteHousing*>(house);

            const auto wine = generateSupply(eResourceType::wine, eh->wine());
            const auto arms = generateSupply(eResourceType::armor, eh->arms());
            const auto horse = generateSupply(eResourceType::horse, eh->horses());

            const auto ww = generateSupplyWidget({wine, arms, horse});
            ww->setY(w->y() + 3*p);
        }
    }

    const auto occ = new eLabel(window());
    occ->setFontSizeS();
    occ->setPaddingS();
    auto occstr = std::to_string(house->people()) + " " +
                  eLanguage::zeusText(127, 15);
    const int vacs = house->vacancies();
    if(vacs > 0) {
        occstr += "  " + eLanguage::zeusText(127, 17) + " " +
                  std::to_string(vacs);
    } else if(vacs < 0) {
        occstr += "  " + std::to_string(-vacs) + " " +
                  eLanguage::zeusText(127, 16);
    }
    occ->setText(occstr);
    occ->fitContent();
    fw->addWidget(occ);
    occ->align(eAlignment::hcenter);

    const auto taxLabel = new eLabel(window());
    {
        const int paid = house->paidTaxes();
        taxLabel->setFontSizeS();
        taxLabel->setPaddingXS();
        taxLabel->setWidth(fw->width());
        taxLabel->setWrapWidth(taxLabel->width());
        std::string taxStr;
        if(paid) {
            taxStr = eLanguage::zeusText(127, 19);
            taxStr += " " + std::to_string(paid) + " ";
            taxStr += eLanguage::zeusText(8, 1) + " ";
            taxStr += eLanguage::zeusText(127, 20);
        } else {
            taxStr = eLanguage::zeusText(127, 18);
        }
        taxLabel->setText(taxStr);
        taxLabel->fitContent();
        fw->addWidget(taxLabel);
        taxLabel->setY(occ->y() + occ->height());
    }

    const auto satLabel = new eLabel(window());
    {
        satLabel->setFontSizeS();
        satLabel->setPaddingXS();
        satLabel->setWidth(fw->width());
        satLabel->setWrapWidth(satLabel->width());
        std::string satstr;
        if(type == eBuildingType::commonHouse) {
            const auto ch = static_cast<SmallHouse*>(house);
            const int sat = ch->satisfaction();
            int n = std::floor((100 - sat)/(100./7));
            n = std::clamp(n, 0, 6);
            satstr = eLanguage::zeusText(127, 21 + n);
        } else { // elite
            satstr = eLanguage::zeusText(127, 21);
        }
        satLabel->setText(satstr);
        satLabel->fitContent();
        fw->addWidget(satLabel);
        satLabel->setY(taxLabel->y() + taxLabel->height());
    }

    if(!house->food()) {
        const auto foodLabel = new eLabel(window());
        foodLabel->setFontSizeS();
        foodLabel->setPaddingXS();
        foodLabel->setWidth(fw->width());
        foodLabel->setWrapWidth(foodLabel->width());
        foodLabel->setText(eLanguage::zeusText(127, 28));
        foodLabel->fitContent();
        fw->addWidget(foodLabel);
        foodLabel->setY(satLabel->y() + satLabel->height());
    }

    {
        const bool atl = house->atlantean();
        const int phi = house->philosophersInventors();
        const int act = house->actorsAstronomers();
        const int ath = house->athletesScholars();
        const int cmp = house->competitorsCurators();
        const int total = house->culturePoints();

        const auto& board = house->getBoard();
        const auto bc = board.boardCityWithId(house->cityId());
        const bool bonus = bc && (atl ? bc->museumBonusActive() : bc->stadiumBonusActive());

        const char* phiA = atl ? "INV" : "PHI";
        const char* actA = atl ? "AST" : "ACT";
        const char* athA = atl ? "SCH" : "ATH";
        const char* cmpA = atl ? "CUR" : "CMP";
        const char* bonA = atl ? "MUS" : "STA";

        auto part = [](const char* nm, int has, int pts) {
            return std::string(nm) + ":" + (has > 0 ? "+" + std::to_string(pts) : std::string("-"));
        };

        std::string s = "Culture: " + std::to_string(total) + "  " +
                        part(phiA, phi, 15) + " " +
                        part(actA, act, 25) + " " +
                        part(athA, ath, 20) + " " +
                        part(cmpA, cmp, 20) + " " +
                        part(bonA, bonus ? 1 : 0, 10);

        const auto l = new eLabel(window());
        l->setFontSizeS();
        l->setPaddingXS();
        l->setWidth(fw->width());
        l->setText(s);
        l->fitContent();
        fw->addWidget(l);
        l->setY(fw->height() - l->height() - p);
        l->align(eAlignment::hcenter);

        const double appeal = house->appeal();
        const bool isElite = type == eBuildingType::eliteHousing;
        const int maxLvl = isElite ? 3 : 6;
        const auto pid = board.cityIdToPlayerId(house->cityId());
        const auto diff = board.difficulty(pid);
        std::string ds = "Desirability: " +
                         std::to_string(static_cast<int>(appeal));
        if(level < maxLvl) {
            const auto req = DifficultyHelpers::houseLevelReq(diff, isElite, level);
            ds += " / evolve " + std::to_string(req.fAppE);
        } else {
            ds += " / max";
        }
        const auto dl = new eLabel(window());
        dl->setFontSizeS();
        dl->setPaddingXS();
        dl->setWidth(fw->width());
        dl->setText(ds);
        dl->fitContent();
        fw->addWidget(dl);
        dl->setY(l->y() - dl->height());
        dl->align(eAlignment::hcenter);
    }
}
