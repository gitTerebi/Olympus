#include "ebuttonutils.h"

#include "framed-button.h"
#include "eframedwidget.h"
#include "emainwindow.h"
#include "ewidget.h"

FramedButton* addPlainButton(const std::string& text,
                              const eAction& a,
                              eWidget* const buttons,
                              eMainWindow* const window) {
    const auto b = new FramedButton(window);
    b->setRenderBg(true);
    b->setUnderline(false);
    b->setPressAction(a);
    b->setText(text);
    b->fitContent();
    buttons->addWidget(b);
    b->align(eAlignment::hcenter);
    return b;
}

FramedButton* addFramedButton(const std::string& text,
                               const eAction& a,
                               eWidget* const buttons,
                               eMainWindow* const window) {
    const auto b = new FramedButton(window);
    b->setRenderBg(true);
    b->setUnderline(false);
    b->setPressAction(a);
    b->setText(text);
    b->fitContent();

    const auto frame = new eFramedWidget(window);
    frame->setType(eFrameType::message);
    const int p = window->resolution().paddingXL();
    frame->resize(b->width() + 2*p, b->height() + 2*p);
    b->move(p, p);
    frame->addWidget(b);
    buttons->addWidget(frame);
    frame->align(eAlignment::hcenter);
    return b;
}
