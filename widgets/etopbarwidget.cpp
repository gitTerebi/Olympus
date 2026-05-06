#include "etopbarwidget.h"

#include "engine/egameboard.h"
#include "engine/boardData/eemploymentdata.h"
#include "engine/boardData/epopulationdata.h"
#include "textures/egametextures.h"
#include "ebutton.h"
#include "edatewidget.h"
#include "egamewidget.h"

#include "emainwindow.h"

#include <string>

namespace {

std::string populationText(const int pop, const eEmploymentData& emplData) {
    return std::to_string(pop);
}

std::string unemployedText(const eEmploymentData& emplData) {
    const int unemployed = emplData.unemployed();
    const int vacancies = emplData.freeJobVacancies();
    std::string result;
    if(unemployed > 0) {
        const int employable = emplData.employable();
        const int percent = employable ? 100*unemployed/employable : 0;
        result += std::to_string(unemployed) + " (" + std::to_string(percent) + "%)";
    }
    if(vacancies > 0) {
        if(!result.empty()) result += " ";
        result += "(-" + std::to_string(vacancies) + ")";
    }
    return result;
}

}

void eTopBarWidget::initialize() {
    const auto& intrfc = eGameTextures::interface();
    const auto uiScale = resolution().uiScale();
    const int icoll = static_cast<int>(uiScale);
    const int mult = icoll + 1;
    const auto& coll = intrfc[icoll];
    setPadding(0);

    const auto s0 = new eWidget(window());
    s0->setWidth(mult*20);

    mDrachmasWidget = new eTopWidget(window());
    mDrachmasWidget->initialize(coll.fDrachmasTopMenu, "-");

    const auto s1 = new eWidget(window());
    s1->setWidth(mult*20);

    mCityLabel = new eLabel("-", window());
    mCityLabel->setFontSizeS();
    mCityLabel->setNoPadding();
    mCityLabel->fitContent();

    const auto s2 = new eWidget(window());
    s2->setWidth(mult*20);

    mPopulationWidget = new eTopWidget(window());
    mPopulationWidget->initialize(coll.fPopulationTopMenu, "-");

    const auto s3 = new eWidget(window());
    s3->setWidth(mult*20);

    mUnemployedWidget = new eTopWidget(window());
    mUnemployedWidget->initialize(coll.fPopulationTopMenu, "-");

    mDateLabel = new eButton(window());
    mDateLabel->setPressAction([this]() {
        if(!mBoard) return;
        const auto dw = new eDateWidget(window());
        dw->initialize([this](const eDate& d) {
            mBoard->setDate(d);
            mDateLabel->setText(d.shortString());
        }, false);
        dw->setDate(mBoard->date());
        window()->execDialog(dw);
        dw->align(eAlignment::center);
    });
    const eDate date(30, eMonth::january, -1500);
    mDateLabel->setFontSizeS();
    mDateLabel->setText(date.shortString());
    mDateLabel->fitContent();
    mDateLabel->setEnabled(false);

    const auto s4 = new eWidget(window());
    s4->setWidth(mult*20);

    const auto s5 = new eWidget(window());
    s5->setWidth(mult*20);

    addWidget(s0);
    addWidget(mCityLabel);
    addWidget(s1);
    addWidget(mDrachmasWidget);
    addWidget(s2);
    addWidget(mPopulationWidget);
    addWidget(s3);
    addWidget(mUnemployedWidget);
    addWidget(s4);
    addWidget(mDateLabel);
    addWidget(s5);

    setHeight(12*mult);

    mCityLabel->align(eAlignment::vcenter);
    mDrachmasWidget->align(eAlignment::vcenter);
    mPopulationWidget->align(eAlignment::vcenter);
    mUnemployedWidget->align(eAlignment::vcenter);
    mDateLabel->align(eAlignment::vcenter);

    layoutHorizontally();
}

void eTopBarWidget::setBoard(eGameBoard* const board) {
    mBoard = board;
}

void eTopBarWidget::setGameWidget(eGameWidget* const gw) {
    mGW = gw;
}

void eTopBarWidget::paintEvent(ePainter& p) {
    // const bool update = (++mTime % 60) == 0;
    if(mBoard) {
        const auto cid = mGW->viewedCity();
        const auto pid = mBoard->personPlayer();
//        const auto pid = mBoard->cityIdToPlayerId(cid);
        const auto& wb = mBoard->world();
        const auto c = wb.cityWithId(cid);

        const auto label = c ? c->name() : "-";
        mCityLabel->setText(label);
        mCityLabel->fitContent();

        const auto popData = mBoard->populationData(cid);
        if(popData) {
            const int pop = popData->population();
            const auto emplData = mBoard->employmentData(cid);
            if(emplData) {
                mPopulationWidget->setText(populationText(pop, *emplData));
                const std::string uText = unemployedText(*emplData);
                mUnemployedWidget->setText(uText);
                if(emplData->unemployed() > 0) {
                    mUnemployedWidget->setIconColor(0, 255, 0); // green
                } else if(emplData->freeJobVacancies() > 0) {
                    mUnemployedWidget->setIconColor(255, 0, 0); // red
                } else {
                    mUnemployedWidget->setIconColor(255, 255, 255); // white
                }
            } else {
                mPopulationWidget->setText(std::to_string(pop));
                mUnemployedWidget->setText("");
            }
        } else {
            mPopulationWidget->setText("-");
            mUnemployedWidget->setText("");
        }

        const int d = mBoard->drachmas(pid);
        mDrachmasWidget->setText(std::to_string(d));

        mDateLabel->setText(mBoard->date().shortString());
        mDateLabel->setEnabled(mBoard->editorMode());

        int iRes;
        int mult;
        iResAndMult(iRes, mult);
        const auto& intrfc = eGameTextures::interface()[iRes];
        const auto& tex = intrfc.fGameTopBar;
        const int texWidth = tex->width();
        const auto& rend = p.renderer();
        bool flip = false;
        for(int x = width() - texWidth; x > -texWidth; x -= texWidth) {
            tex->render(rend, x, 0, flip);
            flip = !flip;
        }
    } else {
        mDateLabel->setEnabled(false);
    }
}
