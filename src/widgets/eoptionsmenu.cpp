#include "eoptionsmenu.h"

#include "eframedwidget.h"
#include "emainwindow.h"
#include "framed-button.h"
#include "elabel.h"
#include "echeckbox.h"
#include "echoosebutton.h"
#include "edifficultywidget.h"
#include "ebasicbutton.h"
#include "ecancelbutton.h"
#include "textures/egametextures.h"
#include "escrollbar.h"


#include <SDL2/SDL.h>

#include <algorithm>
#include <cmath>

class eOptionsSlider : public eWidget {
public:
    using eChangeAction = std::function<void(const int)>;
    using eClamp = std::function<int(const int)>;

    eOptionsSlider(const int min,
                   const int max,
                   const int value,
                   const eClamp& clamp,
                   const eChangeAction& action,
                   eMainWindow* const window) :
        eWidget(window),
        mMin(min),
        mMax(max),
        mClamp(clamp),
        mValue(clampValue(value)),
        mAction(action) {}

protected:
    void paintEvent(ePainter& p) override {
        const int cy = height()/2;
        const SDL_Rect track{0, cy - 2, width(), 4};
        p.fillRect(track, SDL_Color{42, 144, 184, 255});
        p.drawRect(track, SDL_Color{11, 75, 110, 255}, 1);

        const int x = handleX();
        const SDL_Rect handle{x - 5, cy - 10, 10, 20};
        p.fillRect(handle, SDL_Color{172, 226, 236, 255});
        p.drawRect(handle, SDL_Color{6, 55, 80, 255}, 1);
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        setFromX(e.x());
        return true;
    }

    bool mouseMoveEvent(const eMouseEvent& e) override {
        if(static_cast<bool>(e.buttons() & eMouseButton::left)) {
            setFromX(e.x());
        }
        return true;
    }

private:
    int handleX() const {
        const double f = double(mValue - mMin)/double(mMax - mMin);
        return std::round(f*width());
    }

    void setFromX(const int x) {
        const int xx = std::clamp(x, 0, width());
        const double f = double(xx)/double(width());
        const int v = clampValue(std::round(mMin + f*(mMax - mMin)));
        if(v == mValue) return;
        mValue = v;
        if(mAction) mAction(mValue);
    }

    int clampValue(const int value) const {
        if(mClamp) return mClamp(value);
        return std::clamp(value, mMin, mMax);
    }

    const int mMin;
    const int mMax;
    eClamp mClamp;
    int mValue;
    eChangeAction mAction;
};

class eOptionsHotkeyButton : public FramedButton {
public:
    using eChangeAction = std::function<void(const SDL_Scancode)>;

    eOptionsHotkeyButton(const SDL_Scancode value,
                         const eChangeAction& action,
                         eMainWindow* const window) :
        FramedButton(window),
        mValue(value),
        mAction(action) {
        setPadding(2 * resolution().multiplier());
        setUnderline(false);
        setFontSizeXS();
        updateText();
        setWidth(60 * resolution().multiplier());
        setPressAction([this]() {
            mListening = true;
            setText("Press key");
            setTooltip("Press key or DEL to clear");
            fitContent();
            const int minW = 60 * resolution().multiplier();
            if(width() < minW) setWidth(minW);
            grabKeyboard();
        });
    }

protected:
    bool keyPressEvent(const eKeyPressEvent& e) override {
        if(!mListening) return false;
        mListening = false;
        releaseKeyboard();
        if(e.key() == SDL_SCANCODE_DELETE) {
            mValue = SDL_SCANCODE_UNKNOWN;
        } else {
            mValue = e.key();
        }
        updateText();
        if(mAction) mAction(mValue);
        return true;
    }

private:
    void updateText() {
        const char* const name = SDL_GetScancodeName(mValue);
        if(name && name[0]) {
            setText(name);
        } else {
            setText("None");
        }
        fitContent();
        const int minW = 60 * resolution().multiplier();
        if(width() < minW) setWidth(minW);
    }

    SDL_Scancode mValue;
    eChangeAction mAction;
    bool mListening = false;
};

using eOptionsPageViewport = eScrollViewport;

namespace {
void layoutOptionRow(eWidget* const row,
                     eWidget* const label,
                     eWidget* const control,
                     const int rowWidth,
                     const int gap) {
    const int controlW = control->width();
    const int totalW = label->width() + gap + controlW;
    int x = (rowWidth - totalW)/2;
    if(x < 0) x = 0;
    label->setX(x);
    control->setX(x + label->width() + gap);
    const int rowH = std::max(label->height(), control->height());
    row->setHeight(rowH);
    label->setY((rowH - label->height())/2);
    control->setY((rowH - control->height())/2);
}

void clampButtonWidth(FramedButton* const button,
                      const int minW,
                      const int maxW) {
    button->fitContent();
    if(button->width() < minW) button->setWidth(minW);
    if(button->width() > maxW) button->setWidth(maxW);
}
}

eOptionsMenu::eOptionsMenu(const std::vector<ePage>& pages,
                           eMainWindow* const window,
                           const eReopenPage& reopenPage) :
    eModal(window),
    mPages(pages),
    mReopenPage(reopenPage) {}

void eOptionsMenu::initialize(const int initialPage) {
    if(initialPage >= 0 && initialPage < static_cast<int>(mPages.size())) {
        mCurrentPage = initialPage;
    }
    const int mult = resolution().multiplier();
    const int pad = 100 * mult;
    const int sw = std::max(520*mult, 3*resolution().width()/5);
    const int sh = std::max(300*mult, 4*resolution().height()/5);
    const int p0 = padding();
    const int fw = std::min(sw, resolution().width() - 2*p0);
    const int fh = std::min(sh, resolution().height() - 2*p0);
    initializeMask(fw, fh);

    const auto f = frame();
    const int p = f->padding();

    const auto cancel = new eCancelButton(window());
    f->addWidget(cancel);
    cancel->align(eAlignment::bottom | eAlignment::right);
    cancel->move(cancel->x() - 2*p, cancel->y() - 2*p);
    cancel->setPressAction([this]() { close(); });

    mMainTitle = new eLabel("General Options", window());
    mMainTitle->setFontSizeXL();
    mMainTitle->fitContent();
    f->addWidget(mMainTitle);
    mMainTitle->align(eAlignment::hcenter);
    mMainTitle->setY(p);

    const int contentY = mMainTitle->y() + mMainTitle->height() + p;
    const int contentH = f->height() - contentY - 2*p;

    const auto categories = new eWidget(window());
    categories->setNoPadding();
    categories->resize(140*mult, contentH);
    f->addWidget(categories);
    categories->move(2*p, contentY);

    for(int i = 0; i < static_cast<int>(mPages.size()); i++) {
        const auto button = new FramedButton(window());
        button->setUnderline(false);
        button->setText(mPages[i].fButtonLabel);
        button->fitContent();
        button->setPressAction([this, i]() {
            showPage(i);
        });
        categories->addWidget(button);
    }
    categories->stackVertically(p);

    const auto sidebar = new eScrollBar(window());
    sidebar->initialize(contentH);

    const int vpLeft = categories->x() + categories->width() + 2*p;
    const int vpRight = f->width() - p - sidebar->width() - p - pad;

    mPageViewport = new eOptionsPageViewport(window());
    mPageViewport->setNoPadding();
    mPageViewport->resize(vpRight - p - vpLeft, contentH);
    f->addWidget(mPageViewport);
    mPageViewport->move(vpLeft, categories->y());

    sidebar->move(vpRight + p, mPageViewport->y());
    f->addWidget(sidebar);
    sidebar->setViewport(mPageViewport);

    mPage = new eWidget(window());
    mPage->setNoPadding();
    mPage->setWidth(mPageViewport->width());
    mPageViewport->setPage(mPage);

    showPage(mCurrentPage);
}

void eOptionsMenu::showPage(const int id) {
    clearPage();
    if(id < 0 || id >= static_cast<int>(mPages.size())) return;
    mCurrentPage = id;

    const int p = frame()->padding();
    auto& page = mPages[id];
    const auto& settings = window()->settings();

    // Update page values with current settings
    if(page.fButtonLabel == "General") {
        if(!page.fSliders.empty()) page.fSliders[0].fValue = settings.fKeyScrollSpeed;
        if(page.fCheckboxes.size() >= 1) page.fCheckboxes[0].fValue = settings.fDisableEdgeScroll;
    } else if(page.fButtonLabel == "Display") {
        for(auto& choice : page.fChoices) {
            if(choice.fLabel == "Display") {
                choice.fValue = static_cast<int>(settings.fDisplayMode);
            } else if(choice.fLabel == "Filter") {
                choice.fValue = static_cast<int>(settings.fInterpolation);
            } else if(choice.fLabel == "Upscale") {
                choice.fValue = static_cast<int>(settings.fUpscale);
            } else if(choice.fLabel == "Upscale factor") {
                choice.fValue = settings.fUpscaleFactor >= 4 ? 2 :
                    (settings.fUpscaleFactor >= 3 ? 1 : 0);
            }
        }
    } else if(page.fButtonLabel == "Hotkeys") {
        page.fHotkeys[0].fValue = settings.fHotkeyGameMenu;
        page.fHotkeys[1].fValue = settings.fHotkeyPause;
        page.fHotkeys[3].fValue = settings.fHotkeySpeedUp;
        page.fHotkeys[4].fValue = settings.fHotkeySpeedDown;
        page.fHotkeys[6].fValue = settings.fHotkeyRotatePreview;
        page.fHotkeys[7].fValue = settings.fHotkeyCopyBuilding;
        page.fHotkeys[8].fValue = settings.fHotkeyDeleteTool;
        page.fHotkeys[9].fValue = settings.fHotkeyRepairTool;
        page.fHotkeys[10].fValue = settings.fHotkeyUndo;
        page.fHotkeys[12].fValue = settings.fHotkeyBuildRoad;
        page.fHotkeys[13].fValue = settings.fHotkeyBuildRoadblock;
        page.fHotkeys[14].fValue = settings.fHotkeyBuildMaintenanceOffice;
        page.fHotkeys[15].fValue = settings.fHotkeyBuildCommonHousing;
        page.fHotkeys[16].fValue = settings.fHotkeyBuildWatchpost;
        page.fHotkeys[17].fValue = settings.fHotkeyBuildStamp;
        page.fHotkeys[18].fValue = settings.fHotkeyStampManager;
        page.fHotkeys[19].fValue = settings.fHotkeyShowRoadsOverlay;
        page.fHotkeys[21].fValue = settings.fHotkeyScrollLeft;
        page.fHotkeys[22].fValue = settings.fHotkeyScrollRight;
        page.fHotkeys[23].fValue = settings.fHotkeyScrollUp;
        page.fHotkeys[24].fValue = settings.fHotkeyScrollDown;
        page.fHotkeys[26].fValue = settings.fHotkeyBookmark1;
        page.fHotkeys[27].fValue = settings.fHotkeyBookmark2;
        page.fHotkeys[28].fValue = settings.fHotkeyBookmark3;
        page.fHotkeys[29].fValue = settings.fHotkeyBookmark4;
        page.fHotkeys[31].fValue = settings.fHotkeyMenuTab1;
        page.fHotkeys[32].fValue = settings.fHotkeyMenuTab2;
        page.fHotkeys[33].fValue = settings.fHotkeyMenuTab3;
        page.fHotkeys[34].fValue = settings.fHotkeyMenuTab4;
        page.fHotkeys[35].fValue = settings.fHotkeyMenuTab5;
        page.fHotkeys[36].fValue = settings.fHotkeyMenuTab6;
        page.fHotkeys[37].fValue = settings.fHotkeyMenuTab7;
        page.fHotkeys[38].fValue = settings.fHotkeyMenuTab8;
        page.fHotkeys[39].fValue = settings.fHotkeyMenuTab9;
        page.fHotkeys[40].fValue = settings.fHotkeyMenuTab10;
        page.fHotkeys[41].fValue = settings.fHotkeyMenuTab11;
    } else if(page.fButtonLabel == "Gameplay") {
        if(page.fCheckboxes.size() >= 1) page.fCheckboxes[0].fValue = settings.fWarehouseDefaultAcceptNone;
        if(page.fCheckboxes.size() >= 2) page.fCheckboxes[1].fValue = settings.fDoubleCartCapacity;
        if(page.fCheckboxes.size() >= 3) page.fCheckboxes[2].fValue = settings.fAgorasTakeFromTradingPosts;
        if(page.fCheckboxes.size() >= 4) page.fCheckboxes[3].fValue = settings.fEnableYearlyAutosaves;
        if(page.fCheckboxes.size() >= 5) page.fCheckboxes[4].fValue = settings.fPopupForInvasion;
        if(page.fCheckboxes.size() >= 6) page.fCheckboxes[5].fValue = settings.fPopupForRequests;
        if(page.fCheckboxes.size() >= 7) page.fCheckboxes[6].fValue = settings.fPopupForTributes;
        if(page.fCheckboxes.size() >= 8) page.fCheckboxes[7].fValue = settings.fPopupForTroops;
    } else if(page.fButtonLabel == "Sound") {
        if(page.fSliders.size() >= 1) page.fSliders[0].fValue = settings.fGeneralVolume;
        if(page.fSliders.size() >= 2) page.fSliders[1].fValue = settings.fMusicVolume;
        if(page.fSliders.size() >= 3) page.fSliders[2].fValue = settings.fVoiceVolume;
        if(page.fSliders.size() >= 4) page.fSliders[3].fValue = settings.fEventVolume;
        if(page.fSliders.size() >= 5) page.fSliders[4].fValue = settings.fAmbientVolume;
    }

    mMainTitle->setText(page.fTitle);
    mMainTitle->fitContent();
    mMainTitle->align(eAlignment::hcenter);

    const auto makeSlider = [this](const std::string& label,
                                    const std::string& suffix,
                                    const int min,
                                    const int max,
                                    const int value,
                                    const eClampInt& clamp,
                                    const std::function<void(int)>& action) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        w->setWidth(mPage->width());

        const auto valueLabel = new eLabel(window());
        valueLabel->setFontSizeS();
        valueLabel->setText(label + ": " + std::to_string(value) + suffix);
        valueLabel->fitContent();
        w->addWidget(valueLabel);

        const auto slider = new eOptionsSlider(
            min,
            max,
            value,
            clamp,
            [label, suffix, action, valueLabel](const int v) {
                if(action) action(v);
                valueLabel->setText(label + ": " + std::to_string(v) + suffix);
                valueLabel->fitContent();
                valueLabel->align(eAlignment::hcenter);
            },
            window());
        slider->resize(220*resolution().multiplier(),
                       24*resolution().multiplier());
        w->addWidget(slider);

        slider->setY(valueLabel->height());
        w->setHeight(valueLabel->height() + slider->height());
        w->setWidth(mPage->width());
        valueLabel->align(eAlignment::hcenter);
        slider->align(eAlignment::hcenter);
        return w;
    };

    const auto makeHotkey = [this](const eOptionsMenu::eHotkeyItem& hotkey) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        w->setWidth(mPage->width());

        const auto label = new eLabel(hotkey.fLabel, window());
        label->setNoPadding();
        label->setFontSizeXS();
        label->fitContent();
        w->addWidget(label);

        if(hotkey.fSet) {
            const auto button = new eOptionsHotkeyButton(
                hotkey.fValue,
                [hotkey](const SDL_Scancode key) {
                    if(hotkey.fSet) hotkey.fSet(hotkey.fId, key);
                },
                window());
            w->addWidget(button);
            w->fitHeight();
            const int rowH = std::max(button->height(), label->height());
            w->setHeight(rowH);
            const int gap = 4 * resolution().multiplier();
            label->setX(mPage->width()/2 - label->width() - gap);
            button->setX(mPage->width()/2 + gap);
            label->setY((rowH - label->height()) / 2);
            button->setY((rowH - button->height()) / 2);
        } else {
            label->setYellowFontColor();
            label->align(eAlignment::hcenter);
            w->setHeight(label->height());
        }
        return w;
    };

    for(const auto& slider : page.fSliders) {
        mPage->addWidget(makeSlider(slider.fLabel,
                                    slider.fSuffix,
                                    slider.fMin,
                                    slider.fMax,
                                    slider.fValue,
                                    slider.fClamp,
                                    slider.fSet));
    }

    for(const auto& hotkey : page.fHotkeys) {
        mPage->addWidget(makeHotkey(hotkey));
    }

    const auto makeCheckbox = [this](const eOptionsMenu::eCheckboxItem& item) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        w->setWidth(mPage->width());

        const auto label = new eLabel(item.fLabel, window());
        label->setFontSizeS();
        label->fitContent();
        w->addWidget(label);

        const auto cb = new eCheckBox(window());
        cb->setNoPadding();
        cb->setChecked(item.fValue);
        cb->setCheckAction([item](const bool b) {
            if(item.fSet) item.fSet(b);
        });
        cb->fitContent();
        if(!item.fTooltip.empty()) {
            cb->setTooltip(item.fTooltip);
        }
        w->addWidget(cb);

        w->fitHeight();
        w->setWidth(mPage->width());
        const double center = mPage->width() * 0.65;
        label->setX(center - label->width());
        cb->setX(center);
        label->setY((w->height() - label->height()) / 2);
        cb->setY((w->height() - cb->height()) / 2);
        return w;
    };

    // A dropdown row: label on the left, a button showing the current option on the
    // right; pressing it pops an eChooseButton list (same as the editor's choosers).
    const auto makeChoice = [this](const eOptionsMenu::eChoiceItem& item) {
        const auto w = new eWidget(window());
        w->setNoPadding();
        w->setWidth(mPage->width());

        const int mult = resolution().multiplier();
        const bool compact = mPage->width() < 300 * mult;

        const auto label = new eLabel(item.fLabel, window());
        if(compact) label->setFontSizeXS();
        else label->setFontSizeS();
        label->fitContent();
        w->addWidget(label);

        const auto button = new FramedButton(window());
        button->setUnderline(false);
        if(compact) button->setFontSizeXS();
        else button->setFontSizeS();
        const int initial = (item.fValue >= 0 &&
                             item.fValue < int(item.fOptions.size())) ? item.fValue : 0;
        button->setText(item.fOptions.empty() ? "" : item.fOptions[initial]);
        const int gap = 8 * mult;
        const int maxControlW = std::max(56 * mult,
                                         mPage->width() - label->width() - gap);
        const int wantedControlW = compact ?
            std::max(90 * mult, mPage->width()/3) :
            std::max(130 * mult, mPage->width()/3);
        const int minControlW = std::min(wantedControlW, maxControlW);
        clampButtonWidth(button, minControlW, maxControlW);
        const auto options = item.fOptions;
        const auto set = item.fSet;
        const auto reloadsUiScale = item.fReloadsUiScale;
        const bool rebuildOnSet = item.fLabel == "Aspect" || reloadsUiScale;
        button->setPressAction([this, button, options, set, rebuildOnSet,
                                reloadsUiScale, minControlW, maxControlW]() {
            const auto choose = new eChooseButton(window());
            const auto act = [this, button, options, set, rebuildOnSet,
                              reloadsUiScale, minControlW, maxControlW](const int val) {
                bool reloadNeeded = false;
                if(reloadsUiScale) {
                    reloadNeeded = reloadsUiScale(val);
                    if(reloadNeeded && mReopenPage) {
                        const int page = mCurrentPage;
                        window()->setAfterMenuLoadingAction([reopenPage = mReopenPage, page]() {
                            reopenPage(page);
                        });
                    }
                }
                if(val >= 0 && val < int(options.size())) {
                    button->setText(options[val]);
                    clampButtonWidth(button, minControlW, maxControlW);
                }
                if(set) set(val);
                if(rebuildOnSet && !reloadNeeded && mReopenPage) {
                    const int page = mCurrentPage;
                    window()->addSlot([reopenPage = mReopenPage, page]() {
                        reopenPage(page);
                    });
                    close();
                }
            };
            choose->initialize(8, options, act);
            window()->execDialog(choose);
            choose->align(eAlignment::center);
        });
        w->addWidget(button);

        w->fitHeight();
        w->setWidth(mPage->width());
        layoutOptionRow(w, label, button, mPage->width(), gap);
        return w;
    };

    for(const auto& diff : page.fDifficulties) {
        const auto w = new eDifficultyWidget(window());
        w->initialize(diff.fGet(), diff.fSet);
        mPage->addWidget(w);
    }

    for(const auto& choice : page.fChoices) {
        mPage->addWidget(makeChoice(choice));
    }

    for(const auto& checkbox : page.fCheckboxes) {
        mPage->addWidget(makeCheckbox(checkbox));
    }

    for(const auto& text : page.fLines) {
        const auto line = new eLabel(text, window());
        line->setFontSizeS();
        line->fitContent();
        mPage->addWidget(line);
    }

    const int rowSpacing = 2 * resolution().multiplier();
    int y = 0;
    for(const auto child : mPage->children()) {
        child->setY(y);
        y += child->height() + rowSpacing;
    }
    mPage->setHeight(y);
    for(const auto child : mPage->children()) {
        child->align(eAlignment::hcenter);
    }
    mPageViewport->scrollToTop();
}

void eOptionsMenu::clearPage() {
    if(!mPage) return;
    mPage->removeChildren();
}

void eOptionsMenu::rebuild() {
    const int page = mCurrentPage;
    resetModal();
    mPageViewport = nullptr;
    mPage = nullptr;
    mMainTitle = nullptr;
    mCurrentPage = page;
    if(parent()) {
        parent()->resize(window()->width(), window()->height());
    }
    initialize(mCurrentPage);
}


