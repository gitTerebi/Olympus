#include "estampmanager.h"

#include "ebuttonbase.h"
#include "ecancelbutton.h"
#include "eframedbutton.h"
#include "eframedwidget.h"
#include "elabel.h"
#include "emainwindow.h"
#include "equestionwidget.h"
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

    const auto createB = new eFramedButton(window());
    createB->setUnderline(false);
    createB->setFontSizeS();
    createB->setPaddingS();
    createB->setText("Create");
    createB->fitContent();
    f->addWidget(createB);
    createB->align(eAlignment::bottom | eAlignment::left);
    createB->move(createB->x() + 2 * p, createB->y() - 2 * p);
    createB->setPressAction([this]()
                            {
        const auto action = mCreateTemplateAction;
        close();
        if(action) action(); });

    const auto deleteB = new eFramedButton(window());
    deleteB->setUnderline(false);
    deleteB->setFontSizeS();
    deleteB->setPaddingS();
    deleteB->setText("Delete");
    deleteB->fitContent();
    f->addWidget(deleteB);
    deleteB->move(createB->x() + createB->width() + p, createB->y());
    deleteB->setPressAction([this]()
                            { deleteSelectedTemplate(); });

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

    mListWidth = vpW - 2 * tp;
    mFilesWidget = new eWidget(window());
    mFilesWidget->setNoPadding();
    rebuildList();
}

void eStampManager::rebuildList()
{
    mButtons.clear();
    if(mFilesWidget) mFilesWidget->removeAllWidgets();

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
    std::sort(paths.begin(), paths.end(),
              [](const fs::path& a, const fs::path& b) {
        const auto an = a.filename().u8string();
        const auto bn = b.filename().u8string();
        const bool at = !an.empty() && an.front() == '~';
        const bool bt = !bn.empty() && bn.front() == '~';
        if(at != bt) return at;
        return an < bn;
    });

    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int icoll = static_cast<int>(uiScale);
    const auto& intrfc = eGameTextures::interface()[icoll];
    const auto popIcon = intrfc.fPopulationTopMenu;
    const auto drachmaIcon = intrfc.fDrachmasTopMenu;
    const int iconH = res.fontSizeS();

    const int pp = res.paddingXXS();
    const int rowH = iconH + 2 * pp;

    // measure icon widths once (same for all rows)
    int popIconW = 0, drIconW = 0;
    {
        const auto tmp = new eScaledTextureLabel(window());
        tmp->setNoPadding();
        tmp->setTexture(popIcon);
        tmp->setDrawHeight(iconH);
        tmp->setFitToDrawSize(true);
        popIconW = tmp->width();

        const auto tmp2 = new eScaledTextureLabel(window());
        tmp2->setNoPadding();
        tmp2->setTexture(drachmaIcon);
        tmp2->setDrawHeight(iconH);
        tmp2->setFitToDrawSize(true);
        drIconW = tmp2->width();
    }

    struct eRowData {
        std::string name, path;
        int pop, cost;
        eButtonBase* b;
        eLabel* nameL;
        eLabel* popL;
        eLabel* costL;
        eScaledTextureLabel* popIconL;
        eScaledTextureLabel* drIconL;
    };

    // first pass: create widgets, measure text widths
    std::vector<eRowData> rows;
    int maxPopW = 0, maxCostW = 0;
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
        b->setNoPadding();

        const auto nameL = new eLabel(name, window());
        nameL->setFontSizeS();
        nameL->setNoPadding();
        nameL->fitContent();
        b->addWidget(nameL);

        const auto popIconL = new eScaledTextureLabel(window());
        popIconL->setNoPadding();
        popIconL->setTexture(popIcon);
        popIconL->setDrawHeight(iconH);
        popIconL->setFitToDrawSize(true);
        b->addWidget(popIconL);

        const auto popL = new eLabel(std::to_string(pop), window());
        popL->setFontSizeS();
        popL->setNoPadding();
        popL->fitContent();
        b->addWidget(popL);

        const auto drIconL = new eScaledTextureLabel(window());
        drIconL->setNoPadding();
        drIconL->setTexture(drachmaIcon);
        drIconL->setDrawHeight(iconH);
        drIconL->setFitToDrawSize(true);
        b->addWidget(drIconL);

        const auto costL = new eLabel(std::to_string(cost), window());
        costL->setFontSizeS();
        costL->setNoPadding();
        costL->fitContent();
        b->addWidget(costL);

        maxPopW  = std::max(maxPopW,  popL->width());
        maxCostW = std::max(maxCostW, costL->width());

        rows.push_back({name, pathString, pop, cost, b, nameL, popL, costL, popIconL, drIconL});
        mButtons.push_back({name, pathString, pop, cost, b, nameL});
    }

    // fixed column positions (right-aligned block)
    // layout: | ... name ... | popIcon | popCol | drIcon | costCol | margin |
    const int margin   = 4 * pp;
    const int costColW = maxCostW;
    const int drColX   = mListWidth - margin - costColW;
    const int drIconX  = drColX - 4 * pp - drIconW;
    const int popColW  = maxPopW;
    const int popColX  = drIconX - 4 * pp - popColW;
    const int popIconX = popColX - pp / 2 - popIconW;

    int y = 0;
    for (auto &row : rows)
    {
        row.b->resize(mListWidth, rowH);

        row.nameL->setX(pp);
        row.popIconL->setX(popIconX);
        row.popL->setX(popColX);
        row.drIconL->setX(drIconX);
        row.costL->setX(drColX);

        // right-align numbers within their column
        row.popL->setX(popColX + popColW - row.popL->width());
        row.costL->setX(drColX + costColW - row.costL->width());

        row.nameL->align(eAlignment::vcenter);
        row.popIconL->align(eAlignment::vcenter);
        row.popL->align(eAlignment::vcenter);
        row.drIconL->align(eAlignment::vcenter);
        row.costL->align(eAlignment::vcenter);

        mFilesWidget->addWidget(row.b);
        row.b->setY(y);
        y += rowH + pp;

        row.b->setPressAction([this, name = row.name, path = row.path]()
                              { selectTemplate(name, path); });
        row.b->setMouseEnterAction([nameL = row.nameL]()
                                   { nameL->setYellowFontColor(); });
        row.b->setMouseLeaveAction([this]()
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

void eStampManager::deleteSelectedTemplate()
{
    if(!mStampTool) return;
    const auto active = mStampTool->templateName();
    for(const auto& entry : mButtons) {
        if(entry.fName != active) continue;
        const auto path = entry.fPath;
        const auto name = entry.fName;
        const auto q = new eQuestionWidget(window());
        const auto acceptA = [this, path]() {
            std::error_code ec;
            fs::remove(path, ec);
            if(ec) {
                printf("Failed to delete stamp template: %s\n", path.c_str());
                return;
            }
            rebuildList();
        };
        std::string msg = "Delete '" + name + "'?";
        q->initialize("Confirm Delete", msg, acceptA, nullptr);
        window()->execDialog(q);
        q->align(eAlignment::center);
        return;
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
