#include "etopbarwidget.h"

#include "engine/game-board.h"
#include "engine/boardData/eemploymentdata.h"
#include "engine/boardData/epopulationdata.h"
#include "textures/game-textures.h"
#include "elayouthelpers.h"
#include "game-widget.h"

#include "main-window.h"

#include <string>
#include <cmath>

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
    const auto& intrfc = GameTextures::interface();
    const int icoll = GameTextures::interfaceTextureId();
    const double mult = topSidebarMult();
    const auto& coll = intrfc[icoll];
    setPadding(0);

    mDrachmasWidget = new eTopWidget(window());
    mDrachmasWidget->initialize(coll.fDrachmasTopMenu, "-");

    mCityLabel = new eLabel("-", window());
    mCityLabel->setFontSizeS();
    mCityLabel->setNoPadding();
    mCityLabel->fitContent();

    mPopulationWidget = new eTopWidget(window());
    mPopulationWidget->initialize(coll.fPopulationTopMenu, "-");

    mUnemployedWidget = new eTopWidget(window());
    mUnemployedWidget->initialize(coll.fPopulationTopMenu, "-");

    mDateLabel = new eLabel("-", window());
    const eDate date(30, eMonth::january, -1500);
    mDateLabel->setFontSizeS();
    mDateLabel->setText(date.shortString());
    mDateLabel->fitContent();

    setHeight(std::round(12*mult));

    mCitySlot = createTopBarSlot(mCityLabel);
    mDrachmasSlot = createTopBarSlot(mDrachmasWidget);
    mPopulationSlot = createTopBarSlot(mPopulationWidget);
    mUnemployedSlot = createTopBarSlot(mUnemployedWidget);
    mDateSlot = createTopBarSlot(mDateLabel);

    mContentWidget = eLayoutHelpers::createFlexContainer(
        window(), width(), height(), eLayoutHelpers::eFlexDirection::row,
        {{mCitySlot, 0, 1}, {mDrachmasSlot, 0, 1},
         {mPopulationSlot, 0, 1}, {mUnemployedSlot, 0, 1},
         {mDateSlot, 0, 1}},
        {.align = eLayoutHelpers::eAlign::stretch});
    addWidget(mContentWidget);

    layoutContent();
}

void eTopBarWidget::setBoard(GameBoard* const board) {
    mBoard = board;
    mLastContentUpdateMs = 0;
}

void eTopBarWidget::setGameWidget(GameWidget* const gw) {
    mGW = gw;
}

eWidget* eTopBarWidget::createTopBarSlot(eWidget* const content) {
    const auto slot = new eWidget(window());
    slot->setNoPadding();
    slot->resize(1, height());
    slot->addWidget(content);
    return slot;
}

void eTopBarWidget::centerSlotContent(eWidget* const slot) {
    if(!slot || slot->children().empty()) return;
    const auto content = slot->children().front();
    content->move((slot->width() - content->width()) / 2,
                  (slot->height() - content->height()) / 2);
}

void eTopBarWidget::layoutContent() {
    if(!mContentWidget) return;
    mContentWidget->resize(width(), height());
    eLayoutHelpers::updateFlexContainerLayout(
        mContentWidget, eLayoutHelpers::eFlexDirection::row,
        {{mCitySlot, 0, 1}, {mDrachmasSlot, 0, 1},
         {mPopulationSlot, 0, 1}, {mUnemployedSlot, 0, 1},
         {mDateSlot, 0, 1}},
        {.align = eLayoutHelpers::eAlign::stretch});
    centerSlotContent(mCitySlot);
    centerSlotContent(mDrachmasSlot);
    centerSlotContent(mPopulationSlot);
    centerSlotContent(mUnemployedSlot);
    centerSlotContent(mDateSlot);
}

void eTopBarWidget::paintEvent(ePainter& p) {
    // const bool update = (++mTime % 60) == 0;
    if(mBoard) {
        const int now = SDL_GetTicks();
        if(mLastContentUpdateMs == 0 || now - mLastContentUpdateMs >= 100) {
            mLastContentUpdateMs = now;
            bool contentChanged = false;
            const auto cid = mGW->viewedCity();
            const auto pid = mBoard->personPlayer();
//            const auto pid = mBoard->cityIdToPlayerId(cid);
            const auto& wb = mBoard->world();
            const auto c = wb.cityWithId(cid);

            const auto label = c ? c->name() : "-";
            if(mCityLabel->text() != label) {
                mCityLabel->setText(label);
                mCityLabel->fitContent();
                contentChanged = true;
            }

            const auto popData = mBoard->populationData(cid);
            if(popData) {
                const int pop = popData->population();
                const auto emplData = mBoard->employmentData(cid);
                if(emplData) {
                    contentChanged |= mPopulationWidget->setText(populationText(pop, *emplData));
                    const std::string uText = unemployedText(*emplData);
                    contentChanged |= mUnemployedWidget->setText(uText);
                    if(emplData->unemployed() > 0) {
                        contentChanged |= mUnemployedWidget->setIconColor(0, 255, 0); // green
                    } else if(emplData->freeJobVacancies() > 0) {
                        contentChanged |= mUnemployedWidget->setIconColor(255, 0, 0); // red
                    } else {
                        contentChanged |= mUnemployedWidget->setIconColor(255, 255, 255); // white
                    }
                } else {
                    contentChanged |= mPopulationWidget->setText(std::to_string(pop));
                    contentChanged |= mUnemployedWidget->setText("");
                }
            } else {
                contentChanged |= mPopulationWidget->setText("-");
                contentChanged |= mUnemployedWidget->setText("");
            }

            const int d = mBoard->drachmas(pid);
            contentChanged |= mDrachmasWidget->setText(std::to_string(d));

            const auto dateText = mBoard->date().shortString();
            if(mDateLabel->text() != dateText) {
                mDateLabel->setText(dateText);
                mDateLabel->fitContent();
                contentChanged = true;
            }
            if(contentChanged) layoutContent();
        }

        int iRes;
        double mult;
        topSidebarIResAndMult(iRes, mult);
        const auto& intrfc = GameTextures::interface()[iRes];
        const auto& tex = intrfc.fGameTopBar;
        const int texWidth = tex->width();
        const int drawWidth = std::round(texWidth*topSidebarTextureScale());
        const int drawHeight = std::round(tex->height()*topSidebarTextureScale());
        const auto& rend = p.renderer();
        const SDL_Rect clipRect{0, 0, width(), height()};
        p.setClipRect(&clipRect);
        bool flip = false;
        const SDL_Rect srcRect{tex->x(), tex->y(), tex->width(), tex->height()};
        for(int x = width() - drawWidth; x > -drawWidth; x -= drawWidth) {
            const SDL_Rect dstRect{x, 0, drawWidth, drawHeight};
            tex->render(rend, srcRect, dstRect, flip);
            flip = !flip;
        }
        p.setClipRect(nullptr);
    }
}
