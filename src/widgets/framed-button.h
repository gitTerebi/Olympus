#ifndef FRAMED_BUTTON_H
#define FRAMED_BUTTON_H

#include "ebutton.h"

class FramedButton : public eButton {
public:
    using eButton::eButton;

    void setRenderBg(const bool r) { mRenderBg = r; }
    void setNoBorder(const bool b) { mNoBorder = b; }
protected:
    void paintEvent(ePainter& p);
private:
    void renderBg(ePainter& p);

    bool mRenderBg = false;
    bool mNoBorder = false;
};

#endif // FRAMED_BUTTON_H
