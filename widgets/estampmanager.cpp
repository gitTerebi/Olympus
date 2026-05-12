#include "estampmanager.h"

#include "ebuttonbase.h"
#include "ecancelbutton.h"
#include "eframedwidget.h"
#include "elabel.h"
#include "emainwindow.h"
#include "escrollbar.h"

#include "egamedir.h"
#include "engine/stamps/estamptool.h"
#include "engine/stamps/estampblueprint.h"
#include "engine/edifficulty.h"
#include "textures/egametextures.h"
#include "elabelbase.h"

#include <algorithm>
#include <filesystem>

int calculateMaxPop(const std::string &path)
{
    std::vector<eStampElement> bp;
    if (!eReadStampBlueprint(path, bp))
        return 0;
    int pop = 0;
    for (const auto &elem : bp)
    {
        if (elem.type == eBuildingType::commonHouse)
        {
            pop += 60;
        }
        else if (elem.type == eBuildingType::eliteHousing)
        {
            pop += 20;
        }
    }
    return pop;
}

namespace fs = std::filesystem;

void eStampManager::initialize(eStampTool *const stampTool, const eDifficulty difficulty)
{
    mStampTool = stampTool;
    mDifficulty = difficulty;

    const auto res = window()->resolution();
    const int ww = res.centralWidgetSmallWidth();
    const int hh = res.centralWidgetSmallHeight();
    initializeMask(ww, hh);

    const auto f = frame();
    const int p = f->padding();

    const auto title = new eLabel("Stamp Manager", window());
    title->fitContent();
    f->addWidget(title);
    title->align(eAlignment::top | eAlignment::hcenter);
    title->setY(title->y() + p);

    const auto closeB = new eCancelButton(window());
    f->addWidget(closeB);
    closeB->align(eAlignment::bottom | eAlignment::right);
    closeB->move(closeB->x() - 2 * p, closeB->y() - 2 * p);
    closeB->setPressAction([this]()
                           { close(); });

    const int vpY = title->y() + title->height() + 2 * p;
    const int vpH = f->height() - vpY - closeB->height() - 4 * p;

    const auto sidebar = new eScrollBar(window());
    sidebar->initialize(vpH);
    const int vpW = ww - 4 * p - sidebar->width() - p;

    const auto innerFrame = new eFramedWidget(window());
    innerFrame->setNoPadding();
    innerFrame->setType(eFrameType::inner);
    innerFrame->resize(vpW, vpH);
    innerFrame->move(2 * p, vpY);
    f->addWidget(innerFrame);

    const int tp = res.paddingS();
    mViewport = new eScrollViewport(window());
    mViewport->setNoPadding();
    mViewport->resize(vpW - 2 * tp, vpH - 2 * tp);
    mViewport->move(tp, tp);
    innerFrame->addWidget(mViewport);

    sidebar->move(2 * p + vpW + p, vpY);
    f->addWidget(sidebar);
    sidebar->setViewport(mViewport);

    mListWidth = vpW;
    mFilesWidget = new eWidget(window());
    mFilesWidget->setNoPadding();
    rebuildList();
}

void eStampManager::rebuildList()
{
    mButtons.clear();

    std::vector<fs::path> paths;
    const auto folder = eGameDir::stampsDir();
    if (fs::exists(folder))
    {
        for (const auto &entry : fs::directory_iterator(folder))
        {
            const auto path = entry.path();
            if (path.extension() != ".txt")
                continue;
            paths.push_back(path);
        }
    }
    std::sort(paths.begin(), paths.end());

    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int icoll = static_cast<int>(uiScale);
    const auto& intrfc = eGameTextures::interface()[icoll];
    const auto popIcon = intrfc.fPopulationTopMenu;
    const auto drachmaIcon = intrfc.fDrachmasTopMenu;
    const int iconH = res.fontSizeS();

    int y = 0;
    for (const auto &path : paths)
    {
        const auto name = path.stem().u8string();
        const auto pathString = path.u8string();
        const int pop = calculateMaxPop(pathString);

        eStampTool tmpTool;
        tmpTool.setTemplate(name, pathString);
        const int cost = tmpTool.estimatedCost(mDifficulty);

        const auto b = new eButtonBase("", window());
        b->setFontSizeS();
        b->setPaddingXXS();
        b->fitContent();

        // name label
        const auto nameL = new eLabel(name, window());
        nameL->setFontSizeS();
        nameL->setNoPadding();
        nameL->fitContent();
        b->addWidget(nameL);

        // pop icon
        const auto popIconL = new eScaledTextureLabel(window());
        popIconL->setNoPadding();
        popIconL->setTexture(popIcon);
        popIconL->setDrawHeight(iconH);
        popIconL->setFitToDrawSize(true);
        b->addWidget(popIconL);

        // pop count
        const auto popL = new eLabel(std::to_string(pop), window());
        popL->setFontSizeS();
        popL->setNoPadding();
        popL->fitContent();
        b->addWidget(popL);

        // drachma icon
        const auto drIconL = new eScaledTextureLabel(window());
        drIconL->setNoPadding();
        drIconL->setTexture(drachmaIcon);
        drIconL->setDrawHeight(iconH);
        drIconL->setFitToDrawSize(true);
        b->addWidget(drIconL);

        // cost label
        const auto costL = new eLabel(std::to_string(cost), window());
        costL->setFontSizeS();
        costL->setNoPadding();
        costL->fitContent();
        b->addWidget(costL);

        // layout children left-to-right
        const int pp = res.paddingXXS();
        int cx = pp;
        nameL->setX(cx); cx += nameL->width() + pp;
        popIconL->setX(cx); cx += popIconL->width() + pp / 2;
        popL->setX(cx); cx += popL->width() + pp;
        drIconL->setX(cx); cx += drIconL->width() + pp / 2;
        costL->setX(cx);

        nameL->align(eAlignment::vcenter);
        popIconL->align(eAlignment::vcenter);
        popL->align(eAlignment::vcenter);
        drIconL->align(eAlignment::vcenter);
        costL->align(eAlignment::vcenter);

        mFilesWidget->addWidget(b);
        b->setY(y);
        y += b->height();

        mButtons.push_back({name, pathString, pop, cost, b, nameL});
        b->setPressAction([this, name, pathString]()
                          { selectTemplate(name, pathString); });
        b->setMouseEnterAction([nameL]()
                               { nameL->setYellowFontColor(); });
        b->setMouseLeaveAction([this]()
                               { updateButtonColors(); });
    }

    if (paths.empty())
    {
        const auto b = new eButtonBase("No stamp templates found", window());
        b->setFontSizeS();
        b->setLightFontColor();
        b->setTextAlignment(eAlignment::left | eAlignment::vcenter);
        b->setNoPadding();
        b->fitContent();
        b->setWidth(mListWidth);
        b->setEnabled(false);
        mFilesWidget->addWidget(b);
    }

    mFilesWidget->fitContent();
    mFilesWidget->setWidth(mListWidth);
    mViewport->setPage(mFilesWidget);
    updateButtonColors();
}

void eStampManager::selectTemplate(const std::string &name,
                                    const std::string &path)
{
    if (!mStampTool)
        return;
    if (mStampTool->setTemplate(name, path))
    {
        updateButtonColors();
        close();
        if (mTemplateSelectedAction)
            mTemplateSelectedAction();
    }
}

void eStampManager::updateButtonColors()
{
    const std::string active = mStampTool ? mStampTool->templateName() : "";
    for (auto &entry : mButtons)
    {
        if (entry.fName == active)
        {
            entry.fNameLabel->setText("> " + entry.fName + " <");
            entry.fNameLabel->setYellowFontColor();
        }
        else
        {
            entry.fNameLabel->setText(entry.fName);
            entry.fNameLabel->setLightFontColor();
        }
        entry.fNameLabel->fitContent();
        entry.fButton->setWidth(mListWidth);
    }
}