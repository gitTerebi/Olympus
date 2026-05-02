#include "eoptionsmenu.h"

#include "eframedbutton.h"
#include "elabel.h"
#include "echeckbox.h"

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

class eOptionsHotkeyButton : public eFramedButton {
public:
    using eChangeAction = std::function<void(const SDL_Scancode)>;

    eOptionsHotkeyButton(const SDL_Scancode value,
                         const eChangeAction& action,
                         eMainWindow* const window) :
        eFramedButton(window),
        mValue(value),
        mAction(action) {
        setUnderline(false);
        updateText();
        setPressAction([this]() {
            mListening = true;
            setText("Press key");
            fitContent();
            grabKeyboard();
        });
    }

protected:
    bool keyPressEvent(const eKeyPressEvent& e) override {
        if(!mListening) return false;
        mListening = false;
        releaseKeyboard();
        mValue = e.key();
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
    }

    SDL_Scancode mValue;
    eChangeAction mAction;
    bool mListening = false;
};

class eOptionsPageViewport : public eWidget {
public:
    using eWidget::eWidget;

    void setPage(eWidget* const page) {
        mPage = page;
        addWidget(mPage);
        clampDY();
    }

    void scrollToTop() {
        mDy = 0;
        clampDY();
    }

    void clampDY() {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        mDy = std::clamp(mDy, 0, maxDy);
        mPage->setY(-mDy);
    }

protected:
    void paintEvent(ePainter& p) override {
        const auto r = rect();
        p.setClipRect(&r);
    }

    void postPaintEvent(ePainter& p) override {
        p.setClipRect(nullptr);
    }

    bool mouseWheelEvent(const eMouseWheelEvent& e) override {
        mDy -= 35*e.dy();
        clampDY();
        return true;
    }

private:
    int mDy = 0;
    eWidget* mPage = nullptr;
};

eOptionsMenu::eOptionsMenu(const std::vector<ePage>& pages,
                           eMainWindow* const window) :
    eFramedWidget(window),
    mPages(pages) {}

void eOptionsMenu::initialize() {
    setType(eFrameType::message);
    const int p = padding();
    const int mult = resolution().multiplier();
    const int w = std::max(520*mult, 4*resolution().width()/5);
    const int h = std::max(300*mult, 4*resolution().height()/5);
    resize(std::min(w, resolution().width() - 2*p),
           std::min(h, resolution().height() - 2*p));

    const auto title = new eLabel("Options", window());
    title->setHugeFontSize();
    title->fitContent();
    addWidget(title);
    title->align(eAlignment::hcenter);
    title->setY(p);

    const auto ok = new eFramedButton(window());
    ok->setUnderline(false);
    ok->setText("OK");
    ok->fitContent();
    ok->setPressAction([this]() {
        deleteLater();
    });
    addWidget(ok);
    ok->align(eAlignment::bottom | eAlignment::hcenter);
    ok->setY(ok->y() - p);

    const int contentY = title->y() + title->height() + p;
    const int contentH = ok->y() - contentY - p;

    const auto categories = new eWidget(window());
    categories->setNoPadding();
    categories->resize(140*mult, contentH);
    addWidget(categories);
    categories->move(2*p, contentY);

    for(int i = 0; i < static_cast<int>(mPages.size()); i++) {
        const auto button = new eFramedButton(window());
        button->setUnderline(false);
        button->setText(mPages[i].fButtonLabel);
        button->fitContent();
        button->setPressAction([this, i]() {
            showPage(i);
        });
        categories->addWidget(button);
    }
    categories->stackVertically(p);

    mPageViewport = new eOptionsPageViewport(window());
    mPageViewport->setNoPadding();
    mPageViewport->resize(
        width() - categories->x() - categories->width() - 4*p, contentH);
    addWidget(mPageViewport);
    mPageViewport->move(categories->x() + categories->width() + 2*p,
                        categories->y());

    mPage = new eWidget(window());
    mPage->setNoPadding();
    mPage->setWidth(mPageViewport->width());
    mPageViewport->setPage(mPage);

    showPage(0);
}

void eOptionsMenu::showPage(const int id) {
    clearPage();
    if(id < 0 || id >= static_cast<int>(mPages.size())) return;

    const int p = padding();
    const auto& page = mPages[id];

    const auto title = new eLabel(page.fTitle, window());
    title->setHugeFontSize();
    title->fitContent();
    mPage->addWidget(title);

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
        valueLabel->setSmallFontSize();
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

        w->stackVertically(padding());
        w->fitContent();
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
        label->setSmallFontSize();
        label->fitContent();
        w->addWidget(label);

        const auto button = new eOptionsHotkeyButton(
            hotkey.fValue,
            [hotkey](const SDL_Scancode key) {
                if(hotkey.fSet) hotkey.fSet(hotkey.fId, key);
            },
            window());
        w->addWidget(button);

        w->layoutHorizontally();
        w->fitHeight();
        w->setWidth(mPage->width());
        label->setX(mPage->width()/2 - label->width() - padding());
        button->setX(mPage->width()/2 + padding());
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
        label->setSmallFontSize();
        label->fitContent();
        w->addWidget(label);

        const auto cb = new eCheckBox(window());
        cb->setNoPadding();
        cb->setChecked(item.fValue);
        cb->setCheckAction([item](const bool b) {
            if(item.fSet) item.fSet(b);
        });
        cb->fitContent();
        w->addWidget(cb);

        w->layoutHorizontally();
        w->fitHeight();
        w->setWidth(mPage->width());
        label->setX(mPage->width()/2 - label->width() - padding());
        cb->setX(mPage->width()/2 + padding());
        label->setY((w->height() - label->height()) / 2);
        cb->setY((w->height() - cb->height()) / 2);
        return w;
    };

    for(const auto& checkbox : page.fCheckboxes) {
        mPage->addWidget(makeCheckbox(checkbox));
    }

    for(const auto& text : page.fLines) {
        const auto line = new eLabel(text, window());
        line->setSmallFontSize();
        line->fitContent();
        mPage->addWidget(line);
    }

    mPage->stackVertically(p);
    mPage->fitHeight();
    title->align(eAlignment::hcenter);
    for(const auto child : mPage->children()) {
        child->align(eAlignment::hcenter);
    }
    mPageViewport->scrollToTop();
}

void eOptionsMenu::clearPage() {
    if(!mPage) return;
    mPage->removeChildren();
}
