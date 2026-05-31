#include "epierdebugwidget.h"

#include "framed-button.h"
#include "buildings/epier.h"

#include <cstdio>
#include <SDL2/SDL_clipboard.h>

static const char* kFacingNames[4] = {"S", "W", "N", "E"};

void ePierDebugWidget::initialize() {
    setNoPadding();

    const int p = padding();

    // 8 rows: 0-3 idle (sOvX/sOvY), 4-7 load (sLoadOvX/sLoadOvY)
    for(int r = 0; r < 8; r++) {
        const bool load = r >= 4;
        const int f = r % 4;
        double* const xs = load ? ePier::sLoadOvX : ePier::sOvX;
        double* const ys = load ? ePier::sLoadOvY : ePier::sOvY;

        const auto row = new eWidget(window());
        row->setNoPadding();
        addWidget(row);

        mLabels[r] = new FramedButton("pier", window());
        mLabels[r]->setUnderline(false);
        mLabels[r]->fitContent();
        row->addWidget(mLabels[r]);
        updateLabel(r);

        const auto bXm = new FramedButton("X-", window());
        bXm->fitContent();
        bXm->setPressAction([this, r, xs, f]() {
            xs[f] -= 0.1;
            updateLabel(r);
        });
        row->addWidget(bXm);

        const auto bXp = new FramedButton("X+", window());
        bXp->fitContent();
        bXp->setPressAction([this, r, xs, f]() {
            xs[f] += 0.1;
            updateLabel(r);
        });
        row->addWidget(bXp);

        const auto bYm = new FramedButton("Y-", window());
        bYm->fitContent();
        bYm->setPressAction([this, r, ys, f]() {
            ys[f] -= 0.1;
            updateLabel(r);
        });
        row->addWidget(bYm);

        const auto bYp = new FramedButton("Y+", window());
        bYp->fitContent();
        bYp->setPressAction([this, r, ys, f]() {
            ys[f] += 0.1;
            updateLabel(r);
        });
        row->addWidget(bYp);

        row->stackHorizontally(p);
        row->fitContent();
    }

    const auto bCopy = new FramedButton("Copy", window());
    bCopy->fitContent();
    bCopy->setPressAction([]() {
        char buf[512];
        snprintf(buf, sizeof(buf),
                 "double ePier::sOvX[4] = {%.2f, %.2f, %.2f, %.2f};\n"
                 "double ePier::sOvY[4] = {%.2f, %.2f, %.2f, %.2f};\n"
                 "double ePier::sLoadOvX[4] = {%.2f, %.2f, %.2f, %.2f};\n"
                 "double ePier::sLoadOvY[4] = {%.2f, %.2f, %.2f, %.2f};\n",
                 ePier::sOvX[0], ePier::sOvX[1], ePier::sOvX[2], ePier::sOvX[3],
                 ePier::sOvY[0], ePier::sOvY[1], ePier::sOvY[2], ePier::sOvY[3],
                 ePier::sLoadOvX[0], ePier::sLoadOvX[1], ePier::sLoadOvX[2], ePier::sLoadOvX[3],
                 ePier::sLoadOvY[0], ePier::sLoadOvY[1], ePier::sLoadOvY[2], ePier::sLoadOvY[3]);
        SDL_SetClipboardText(buf);
    });
    addWidget(bCopy);

    stackVertically(p);
    fitContent();
}

void ePierDebugWidget::updateLabel(const int row) {
    if(row < 0 || row > 7) return;
    if(!mLabels[row]) return;
    const bool load = row >= 4;
    const int f = row % 4;
    const double x = load ? ePier::sLoadOvX[f] : ePier::sOvX[f];
    const double y = load ? ePier::sLoadOvY[f] : ePier::sOvY[f];
    char buf[64];
    snprintf(buf, sizeof(buf), "%s %s X%.2f Y%.2f",
             load ? "load" : "idle", kFacingNames[f], x, y);
    mLabels[row]->setText(buf);
    mLabels[row]->fitContent();
}
