#include "epierdebugwidget.h"

#include "eframedbutton.h"
#include "buildings/epier.h"

#include <cstdio>
#include <SDL2/SDL_clipboard.h>

static const char* kFacingNames[4] = {"BR", "BL", "TL", "TR"};

void ePierDebugWidget::initialize() {
    setNoPadding();

    const int p = padding();

    for(int f = 0; f < 4; f++) {
        const auto row = new eWidget(window());
        row->setNoPadding();
        addWidget(row);

        mLabels[f] = new eFramedButton("pier", window());
        mLabels[f]->setUnderline(false);
        mLabels[f]->fitContent();
        row->addWidget(mLabels[f]);
        updateLabel(f);

        const auto bXm = new eFramedButton("X-", window());
        bXm->fitContent();
        bXm->setPressAction([this, f]() {
            ePier::sOvX[f] -= 0.1;
            updateLabel(f);
        });
        row->addWidget(bXm);

        const auto bXp = new eFramedButton("X+", window());
        bXp->fitContent();
        bXp->setPressAction([this, f]() {
            ePier::sOvX[f] += 0.1;
            updateLabel(f);
        });
        row->addWidget(bXp);

        const auto bYm = new eFramedButton("Y-", window());
        bYm->fitContent();
        bYm->setPressAction([this, f]() {
            ePier::sOvY[f] -= 0.1;
            updateLabel(f);
        });
        row->addWidget(bYm);

        const auto bYp = new eFramedButton("Y+", window());
        bYp->fitContent();
        bYp->setPressAction([this, f]() {
            ePier::sOvY[f] += 0.1;
            updateLabel(f);
        });
        row->addWidget(bYp);

        row->stackHorizontally(p);
        row->fitContent();
    }

    const auto bCopy = new eFramedButton("Copy", window());
    bCopy->fitContent();
    bCopy->setPressAction([]() {
        char buf[256];
        snprintf(buf, sizeof(buf),
                 "double ePier::sOvX[4] = {%.2f, %.2f, %.2f, %.2f};\n"
                 "double ePier::sOvY[4] = {%.2f, %.2f, %.2f, %.2f};\n",
                 ePier::sOvX[0], ePier::sOvX[1], ePier::sOvX[2], ePier::sOvX[3],
                 ePier::sOvY[0], ePier::sOvY[1], ePier::sOvY[2], ePier::sOvY[3]);
        SDL_SetClipboardText(buf);
    });
    addWidget(bCopy);

    stackVertically(p);
    fitContent();
}

void ePierDebugWidget::updateLabel(const int facing) {
    if(facing < 0 || facing > 3) return;
    if(!mLabels[facing]) return;
    char buf[64];
    snprintf(buf, sizeof(buf), "%s X%.2f Y%.2f",
             kFacingNames[facing],
             ePier::sOvX[facing], ePier::sOvY[facing]);
    mLabels[facing]->setText(buf);
    mLabels[facing]->fitContent();
}
