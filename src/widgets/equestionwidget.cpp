#include "equestionwidget.h"

#include "elabel.h"
#include "eacceptbutton.h"
#include "ecancelbutton.h"

namespace {
    void centerLabel(eLabel* const label,
                     const int width) {
        label->setWidth(width);
        label->align(eAlignment::hcenter);
    }
}

void eQuestionWidget::initialize(const std::string& title,
                                 const std::string& text,
                                 const eAction& acceptA,
                                 const eAction& cancelA) {
    initialize(title, text, nullptr, acceptA, cancelA);
}

void eQuestionWidget::initialize(const std::string& title,
                                 const std::string& text,
                                 const stdsptr<eTexture>& icon,
                                 const eAction& acceptA,
                                 const eAction& cancelA) {
    setType(eFrameType::message);
    const int p = padding();
    const int width = 40*p;
    const int hpad = 2*p;
    const auto cw = new eWidget(window());
    cw->setNoPadding();
    cw->setWidth(width);

    const auto titleLabel = new eLabel(window());
    titleLabel->setText(title);
    titleLabel->fitContent();
    titleLabel->setWidth(width);
    cw->addWidget(titleLabel);

    eLabel* iconLabel = nullptr;
    if(icon) {
        iconLabel = new eLabel(window());
        iconLabel->setNoPadding();
        iconLabel->setTexture(icon);
        iconLabel->fitContent();
        cw->addWidget(iconLabel);
    }

    const auto textLabel = new eLabel(window());
    textLabel->setFontSizeS();
    textLabel->setWrapWidth(width - 2*hpad);
    textLabel->setWrapAlignment(eAlignment::hcenter);
    textLabel->setText(text);
    textLabel->fitContent();
    textLabel->setWidth(width);
    cw->addWidget(textLabel);
    int tly = titleLabel->y() + titleLabel->height();
    if(iconLabel) {
        iconLabel->setY(tly + p/2);
        iconLabel->align(eAlignment::hcenter);
        tly = iconLabel->y() + iconLabel->height() + p/2;
    }
    textLabel->setY(tly);

    const auto buttons = new eWidget(window());

    const auto accept = new eAcceptButton(window());
    const auto cancel = new eCancelButton(window());
    accept->setPressAction([this, acceptA]() {
        if(acceptA) acceptA();
        deleteLater();
    });
    cancel->setPressAction([this, cancelA]() {
        if(cancelA) cancelA();
        deleteLater();
    });
    buttons->addWidget(cancel);
    buttons->addWidget(accept);

    buttons->setNoPadding();
    accept->setX(width/5);
    buttons->fitContent();

    cw->addWidget(buttons);
    const int by = textLabel->y() + textLabel->height() + p;
    buttons->setY(by);

    cw->fitContent();
    cw->setWidth(width);
    cw->setHeight(cw->height() + p);
    addWidget(cw);
    cw->move(p, p);
    resize(cw->width() + 2*p, cw->height() + 2*p);
    centerLabel(titleLabel, cw->width());
    centerLabel(textLabel, cw->width());
    if(iconLabel) iconLabel->align(eAlignment::hcenter);
    buttons->align(eAlignment::hcenter);
}
