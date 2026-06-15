#include "eappealdatawidget.h"

#include "eviewmodebutton.h"

#include "widgets/game-widget.h"
#include "widgets/emultilinelabel.h"
#include "engine/game-board.h"
#include "vector-helpers.h"
#include "buildings/eaestheticsbuilding.h"

#include "language.h"
#include "string-helpers.h"

void eAppealDataWidget::initialize() {
    mSeeAppeal = new eViewModeButton(
                     Language::zeusText(14, 17), // see appeal
                     eViewMode::appeal,
                     window());
    addViewButton(mSeeAppeal);

    eDataWidget::initialize();

    const auto inner = innerWidget();
    const int iw = inner->width();

    const auto title = new eLabel(window());
    title->setNoPadding();
    title->setFontSizeXS();
    title->setWrapWidth(iw);
    title->setWrapAlignment(Alignment::hcenter);
    title->setText(Language::zeusText(133, 1)); // commemorative monuments
    title->fitContent();
    inner->addWidget(title);

    mMonumentsWidget = new eWidget(window());
    mMonumentsWidget->setNoPadding();
    const int innerW = inner->width();
    mMonumentsWidget->setWidth(innerW);
    inner->addWidget(mMonumentsWidget);

    inner->stackVertically();
    title->align(Alignment::hcenter);

    mNoMonumentsWidget = new eLabel(window());
    mNoMonumentsWidget->setWrapWidth(iw);
    mNoMonumentsWidget->setWrapAlignment(Alignment::hcenter);
    mNoMonumentsWidget->setYellowFontColor();
    mNoMonumentsWidget->setNoPadding();
    mNoMonumentsWidget->setFontSizeXS();
    const auto text = Language::zeusText(133, 2); // no commemorative monuments
    mNoMonumentsWidget->setText(text);
    mNoMonumentsWidget->fitContent();
    inner->addWidget(mNoMonumentsWidget);
    mNoMonumentsWidget->setY(mMonumentsWidget->y());
    mNoMonumentsWidget->align(Alignment::hcenter);
}

class eMonumentButton : public eButtonBase {
public:
    using eButtonBase::eButtonBase;

    void initialize(const std::string& txt,
                    const std::vector<eTile*>& tiles,
                    GameWidget* const gw) {
        mTiles = tiles;

        setYellowFontColor();
        setNoPadding();
        setFontSizeXS();
        setText(txt);
        fitContent();

        setMouseEnterAction([this]() {
            setLightFontColor();
        });
        setMouseLeaveAction([this]() {
            setYellowFontColor();
        });
        setPressAction([this, gw]() {
            const int ts = mTiles.size();
            if(mId >= ts) mId = 0;
            const auto tile = mTiles[mId];
            gw->viewTile(tile);
            mId++;
        });
    }
private:
    int mId = 0;
    std::vector<eTile*> mTiles;
};

void eAppealDataWidget::paintEvent(ePainter& p) {
    const bool update = ((mTime++) % 20) == 0;
    if(update) {
        const auto cid = viewedCity();
        const auto& bs = mBoard.commemorativeBuildings(cid);
        const bool changed = !VectorHelpers::same(bs, mBuildings);
        if(changed) {
            mNoMonumentsWidget->setVisible(bs.empty());
            mBuildings = bs;
            mMonumentsWidget->removeAllWidgets();
            const auto gw = gameWidget();

            const auto templ1 = Language::zeusText(133, 3); // [commemorative_monument]
            const auto templ2 = Language::zeusText(133, 4); // [commemorative_monument] ([amount])

            const auto commParser = [&](const int id, const int string) {
                std::vector<eTile*> tiles;
                for(const auto b : mBuildings) {
                    const auto bt = b->type();
                    if(bt != eBuildingType::commemorative) continue;
                    const auto c = static_cast<eCommemorative*>(b);
                    const int cid = c->id();
                    if(cid == id) {
                        const auto tile = b->centerTile();
                        tiles.push_back(tile);
                    }
                }
                if(tiles.empty()) return;

                std::string title;
                const auto count = tiles.size();
                if(count > 1) {
                    title = templ2;
                    const auto countStr = std::to_string(count);
                    StringHelpers::replace(title, "[amount]", countStr);
                } else {
                    title = templ1;
                }
                const auto monStr = Language::zeusText(133, string);
                StringHelpers::replace(title, "[commemorative_monument]", monStr);

                const auto w = new eMonumentButton(window());
                w->initialize(title, tiles, gw);
                mMonumentsWidget->addWidget(w);
                w->align(Alignment::hcenter);
            };

            commParser(0, 5); // population
            commParser(1, 7); // victory
            commParser(2, 10); // colony
            commParser(3, 13); // athelete
            commParser(4, 8); // conquest
            commParser(5, 11); // happiness
            commParser(6, 6); // heroic
            commParser(7, 9); // diplomacy
            commParser(8, 12); // scholar

            std::map<GodType, std::vector<eTile*>> gods;
            for(const auto b : mBuildings) {
                const auto bt = b->type();
                if(bt != eBuildingType::godMonument) continue;
                const auto c = static_cast<eGodMonument*>(b);
                const auto tile = c->centerTile();
                const auto god = c->god();
                gods[god].push_back(tile);
            }
            for(const auto& gg : gods) {
                const auto g = gg.first;
                const auto& tiles = gg.second;
                const auto count = tiles.size();
                std::string title;
                if(count > 1) {
                    title = templ2;
                    const auto countStr = std::to_string(count);
                    StringHelpers::replace(title, "[amount]", countStr);
                } else {
                    title = templ1;
                }
                const auto name = God::sGodName(g);
                StringHelpers::replace(title, "[commemorative_monument]", name);

                const auto w = new eMonumentButton(window());
                w->initialize(title, tiles, gw);
                mMonumentsWidget->addWidget(w);
                w->align(Alignment::hcenter);
            }

            mMonumentsWidget->stackVertically();
            mMonumentsWidget->fitHeight();
        }
    }
    eWidget::paintEvent(p);
}
