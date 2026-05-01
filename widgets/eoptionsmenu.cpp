#include "eoptionsmenu.h"

#include "eframedbutton.h"
#include "elabel.h"
#include "emainwindow.h"
#include "esettings.h"

#include <algorithm>
#include <cmath>

class eKeyScrollSpeedSlider : public eWidget {
public:
    using eChangeAction = std::function<void(const int)>;

    eKeyScrollSpeedSlider(const int value,
                          const eChangeAction& action,
                          eMainWindow* const window) :
        eWidget(window),
        mValue(eSettings::clampKeyScrollSpeed(value)),
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
        const double f = double(mValue - eSettings::sMinKeyScrollSpeed)/
                         double(eSettings::sMaxKeyScrollSpeed -
                                eSettings::sMinKeyScrollSpeed);
        return std::round(f*width());
    }

    void setFromX(const int x) {
        const int xx = std::clamp(x, 0, width());
        const double f = double(xx)/double(width());
        const int range = eSettings::sMaxKeyScrollSpeed -
                          eSettings::sMinKeyScrollSpeed;
        const int v = std::round(eSettings::sMinKeyScrollSpeed + f*range);
        if(v == mValue) return;
        mValue = v;
        if(mAction) mAction(mValue);
    }

    int mValue;
    eChangeAction mAction;
};

eOptionsMenu::eOptionsMenu(const eGetInt& getKeyScrollSpeed,
                           const eSetInt& setKeyScrollSpeed,
                           eMainWindow* const window) :
    eFramedWidget(window),
    mGetKeyScrollSpeed(getKeyScrollSpeed),
    mSetKeyScrollSpeed(setKeyScrollSpeed) {}

void eOptionsMenu::initialize() {
    setType(eFrameType::message);
    const int p = padding();
    resize(520*resolution().multiplier(), 300*resolution().multiplier());

    const auto title = new eLabel("Options", window());
    title->setHugeFontSize();
    title->fitContent();
    addWidget(title);
    title->align(eAlignment::hcenter);
    title->setY(p);

    const auto categories = new eWidget(window());
    categories->setNoPadding();
    categories->resize(140*resolution().multiplier(),
                       height() - 4*p - title->height());
    addWidget(categories);
    categories->move(2*p, title->y() + title->height() + p);

    const auto display = new eFramedButton(window());
    display->setUnderline(false);
    display->setText("Display");
    display->fitContent();
    display->setPressAction([this]() {
        showDisplay();
    });
    categories->addWidget(display);

    const auto hotkeys = new eFramedButton(window());
    hotkeys->setUnderline(false);
    hotkeys->setText("Hotkeys");
    hotkeys->fitContent();
    hotkeys->setPressAction([this]() {
        showHotkeys();
    });
    categories->addWidget(hotkeys);
    categories->stackVertically(p);

    mPage = new eWidget(window());
    mPage->setNoPadding();
    mPage->resize(width() - categories->x() - categories->width() - 4*p,
                  categories->height());
    addWidget(mPage);
    mPage->move(categories->x() + categories->width() + 2*p,
                categories->y());

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

    showDisplay();
}

void eOptionsMenu::showDisplay() {
    clearPage();
    const int p = padding();

    const auto title = new eLabel("Display Options", window());
    title->setHugeFontSize();
    title->fitContent();
    mPage->addWidget(title);

    const int speed = mGetKeyScrollSpeed ? mGetKeyScrollSpeed() : 18;
    const auto valueLabel = new eLabel(window());
    valueLabel->setSmallFontSize();
    valueLabel->setText("Key scroll speed: " + std::to_string(speed));
    valueLabel->fitContent();
    mPage->addWidget(valueLabel);

    const auto slider = new eKeyScrollSpeedSlider(
        speed,
        [this, valueLabel](const int v) {
            if(mSetKeyScrollSpeed) mSetKeyScrollSpeed(v);
            valueLabel->setText("Key scroll speed: " + std::to_string(v));
            valueLabel->fitContent();
            valueLabel->align(eAlignment::hcenter);
        },
        window());
    slider->resize(220*resolution().multiplier(),
                   24*resolution().multiplier());
    mPage->addWidget(slider);

    mPage->stackVertically(p);
    title->align(eAlignment::hcenter);
    valueLabel->align(eAlignment::hcenter);
    slider->align(eAlignment::hcenter);
}

void eOptionsMenu::showHotkeys() {
    clearPage();
    const int p = padding();

    const auto title = new eLabel("Hotkeys", window());
    title->setHugeFontSize();
    title->fitContent();
    mPage->addWidget(title);

    const auto line = new eLabel("W/A/S/D: smooth scroll map", window());
    line->setSmallFontSize();
    line->fitContent();
    mPage->addWidget(line);

    mPage->stackVertically(p);
    title->align(eAlignment::hcenter);
    line->align(eAlignment::hcenter);
}

void eOptionsMenu::clearPage() {
    if(!mPage) return;
    mPage->removeChildren();
}
