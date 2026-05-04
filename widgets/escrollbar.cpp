#include "escrollbar.h"

#include "escrollwidget.h"

void eScrollBar::setScrollWidget(eScrollWidget* const sw) {
    mSw = sw;
}

void eScrollBar::scrollUp() {
    if(mSw) mSw->scrollUp();
}

void eScrollBar::scrollDown() {
    if(mSw) mSw->scrollDown();
}
