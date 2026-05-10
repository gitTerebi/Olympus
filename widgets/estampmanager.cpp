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

void eStampManager::initialize(eStampTool *const stampTool)
{
    mStampTool = stampTool;

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

    int y = 0;
    for (const auto &path : paths)
    {
        const auto name = path.stem().u8string();
        const auto pathString = path.u8string();
        const int pop = calculateMaxPop(pathString);
        const auto buttonText = name + " (" + std::to_string(pop) + ")";
        const auto b = new eButtonBase(buttonText, window());
        b->setFontSizeS();
        b->setTextAlignment(eAlignment::left | eAlignment::vcenter);
        b->setPaddingXXS();
        b->fitContent();
        mFilesWidget->addWidget(b);
        b->setY(y);
        y += b->height();

        mButtons.push_back({name, pathString, pop, b});
        b->setPressAction([this, name, pathString]()
                          { selectTemplate(name, pathString); });
        b->setMouseEnterAction([b]()
                               { b->setYellowFontColor(); });
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
        const auto buttonText = entry.fName + " (" + std::to_string(entry.fPop) + ")";
        if (entry.fName == active)
        {
            entry.fButton->setText("> " + buttonText + " <");
            entry.fButton->setYellowFontColor();
        }
        else
        {
            entry.fButton->setText(buttonText);
            entry.fButton->setLightFontColor();
        }
        entry.fButton->fitContent();
        entry.fButton->setWidth(mListWidth);
    }
}