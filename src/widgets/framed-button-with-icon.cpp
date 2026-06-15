#include "framed-button-with-icon.h"

#include "engine/eresourcetype.h"

void FramedButtonWithIcon::initialize(const std::shared_ptr<Texture>& icon,
                                      const std::string& text) {
    const auto r = resolution();
    const double mult = r.multiplier();

    const int p = std::round(13*mult);
    const int w = r.centralWidgetSmallWidth();
    setWidth(w);
    setHeight(2*p);
    setNoPadding();

    const auto iconLabel = new eLabel(window());
    iconLabel->setNoPadding();
    iconLabel->setTexture(icon);
    iconLabel->fitContent();
    addWidget(iconLabel);

    const auto textLabel = new eLabel(window());
    textLabel->setNoPadding();
    textLabel->setFontSizeXS();
    textLabel->setText(text);
    textLabel->fitContent();
    addWidget(textLabel);

    iconLabel->setX(p);
    textLabel->setX(iconLabel->x() + iconLabel->width() + p);

    iconLabel->align(Alignment::vcenter);
    textLabel->align(Alignment::vcenter);
}

void FramedButtonWithIcon::initialize(const eResourceType type,
                                      const std::string& text) {
    const auto r = resolution();
    const auto uiScale = r.uiScale();
    const auto icon = eResourceTypeHelpers::icon(uiScale, type);
    initialize(icon, text);
}
