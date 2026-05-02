#include "eoptionsmenu.h"

#include "ecancelbutton.h"
#include "eframedbutton.h"
#include "elabel.h"
#include "echeckbox.h"
#include "ebasicbutton.h"
#include "textures/egametextures.h"

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
        setPadding(2 * resolution().multiplier());
        setUnderline(false);
        setVerySmallFontSize();
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

class eOptionsScrollThumb : public eWidget {
public:
    using eScrollAction = std::function<void(int dy)>;

    eOptionsScrollThumb(eMainWindow* const window) : eWidget(window) {}

    void setScrollAction(const eScrollAction& a) { mScrollAction = a; }

protected:
    void paintEvent(ePainter& p) override {
        int iRes, mult;
        iResAndMult(iRes, mult);
        const int dim = 8 * mult;
        const auto& intrfc = eGameTextures::interface()[iRes];
        if(!intrfc.fLoaded) return;
        const auto& coll = mDragging ? intrfc.fButtonFrameHover : intrfc.fButtonFrame;
        const int iMax = width() / dim + 1;
        const int jMax = height() / dim + 1;
        const int lastX = width() - dim;
        const int lastY = height() - dim;
        for(int i = 0; i < iMax; i++) {
            const int x = i == iMax - 1 ? lastX : dim * i;
            for(int j = 0; j < jMax; j++) {
                int texId;
                if(i == 0) texId = (j == 0) ? 0 : (j == jMax-1) ? 6 : 7;
                else if(i == iMax-1) texId = (j == 0) ? 2 : (j == jMax-1) ? 4 : 3;
                else if(j == 0) texId = 1;
                else if(j == jMax-1) texId = 5;
                else continue;
                const int y = j == jMax - 1 ? lastY : dim * j;
                p.drawTexture(x, y, coll.getTexture(texId));
            }
        }
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        mDragY = e.y();
        mDragging = true;
        return true;
    }

    bool mouseReleaseEvent(const eMouseEvent&) override {
        mDragging = false;
        return true;
    }

    bool mouseMoveEvent(const eMouseEvent& e) override {
        if(!mDragging) return false;
        const int delta = e.y() - mDragY;
        mDragY = e.y();
        if(mScrollAction) mScrollAction(delta);
        return true;
    }

private:
    bool mDragging = false;
    int mDragY = 0;
    eScrollAction mScrollAction;
};

class eOptionsPageViewport : public eWidget {
public:
    using eScrollCallback = std::function<void(int dy, int maxDy)>;

    using eWidget::eWidget;

    void setPage(eWidget* const page) {
        mPage = page;
        addWidget(mPage);
        clampDY();
    }

    void setScrollCallback(const eScrollCallback& cb) { mScrollCb = cb; }

    void scrollToTop() {
        mDy = 0;
        clampDY();
    }

    void scrollUp() {
        mDy -= 35 * resolution().multiplier();
        clampDY();
    }

    void scrollDown() {
        mDy += 35 * resolution().multiplier();
        clampDY();
    }

    void scrollByPixels(const int delta, const int trackH, const int thumbH) {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        if(thumbH >= trackH || maxDy <= 0) return;
        const float ratio = static_cast<float>(maxDy) / (trackH - thumbH);
        mDy += static_cast<int>(delta * ratio);
        clampDY();
    }

    void clampDY() {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        mDy = std::clamp(mDy, 0, maxDy);
        mPage->setY(-mDy);
        if(mScrollCb) mScrollCb(mDy, maxDy);
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
        mDy -= 35 * resolution().multiplier() * e.dy();
        clampDY();
        return true;
    }

private:
    int mDy = 0;
    eWidget* mPage = nullptr;
    eScrollCallback mScrollCb;
};

eOptionsMenu::eOptionsMenu(const std::vector<ePage>& pages,
                           eMainWindow* const window) :
    eFramedWidget(window),
    mPages(pages) {}

void eOptionsMenu::initialize() {
    setType(eFrameType::message);
    const int p = padding();
    const int mult = resolution().multiplier();
    const int pad = 100 * mult;
    const int w = std::max(520*mult, 4*resolution().width()/5);
    const int h = std::max(300*mult, 4*resolution().height()/5);
    resize(std::min(w, resolution().width() - 2*p),
           std::min(h, resolution().height() - 2*p));

      mMainTitle = new eLabel("General Options", window());
      mMainTitle->setHugeFontSize();
      mMainTitle->fitContent();
      addWidget(mMainTitle);
      mMainTitle->align(eAlignment::hcenter);
      mMainTitle->setY(p);

      const auto ok = new eCancelButton(window());
      ok->fitContent();
      ok->setPressAction([this]() {
          deleteLater();
      });
      addWidget(ok);
      ok->setX(width() - ok->width() - (p + 20));
      ok->setY(height() - ok->height() - (p + 20));

      const int contentY = mMainTitle->y() + mMainTitle->height() + p;
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

    const auto upBtn = new eBasicButton(&eInterfaceTextures::fBigUpButton, window());
    const auto downBtn = new eBasicButton(&eInterfaceTextures::fBigDownButton, window());
    const int sbW = upBtn->width() / 2;
    upBtn->resize(sbW, upBtn->height() / 2);
    downBtn->resize(sbW, downBtn->height() / 2);

    const int vpLeft = categories->x() + categories->width() + 2*p;
    const int vpRight = width() - p - sbW - p - pad;

    mPageViewport = new eOptionsPageViewport(window());
    mPageViewport->setNoPadding();
    mPageViewport->resize(vpRight - p - vpLeft, contentH);
    addWidget(mPageViewport);
    mPageViewport->move(vpLeft, categories->y());

    const int trackTop = mPageViewport->y() + upBtn->height();
    const int trackH = contentH - upBtn->height() - downBtn->height();

    const auto thumb = new eOptionsScrollThumb(window());
    thumb->setNoPadding();
    thumb->setWidth(sbW);
    thumb->setHeight(trackH);
    thumb->move(vpRight + p, trackTop);
    addWidget(thumb);

    addWidget(upBtn);
    addWidget(downBtn);
    upBtn->move(vpRight + p, mPageViewport->y());
    downBtn->move(vpRight + p, mPageViewport->y() + contentH - downBtn->height());
    upBtn->setPressAction([this]() { mPageViewport->scrollUp(); });
    downBtn->setPressAction([this]() { mPageViewport->scrollDown(); });

    thumb->setScrollAction([this, thumb, trackH](const int delta) {
        const int thumbH = thumb->height();
        mPageViewport->scrollByPixels(delta, trackH, thumbH);
    });

    mPageViewport->setScrollCallback([thumb, trackH, trackTop](const int dy, const int maxDy) {
        if(maxDy <= 0) {
            thumb->setHeight(trackH);
            thumb->setY(trackTop);
            return;
        }
        const int thumbH = std::max(20, trackH * trackH / (trackH + maxDy));
        const int thumbY = static_cast<int>(static_cast<float>(dy) / maxDy * (trackH - thumbH));
        thumb->setHeight(thumbH);
        thumb->setY(trackTop + thumbY);
    });

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
        label->setVerySmallFontSize();
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

        w->fitHeight();
        w->setWidth(mPage->width());
        label->setX(mPage->width()/2 - label->width());
        cb->setX(mPage->width()/2);
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

bool eOptionsMenu::keyPressEvent(const eKeyPressEvent& e) {
    if(e.key() == SDL_SCANCODE_ESCAPE) {
        deleteLater();
        return true;
    }
    return false;
}
