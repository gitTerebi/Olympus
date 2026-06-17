#ifndef FRAMED_BUTTON_H
#define FRAMED_BUTTON_H

#include "ebutton.h"

class FramedButton : public eButton {
public:
    using eButton::eButton;

    void renderTargetsReset() override;
    void setRenderBg(const bool r);
    void setNoBorder(const bool b);
protected:
    void paintEvent(ePainter& p) override;
private:
    void renderBg(ePainter& p);
    void resetFrameCache();
    std::shared_ptr<Texture>& frameCache(bool hover);
    void renderFrameCache(ePainter& p, bool hover);

    bool mRenderBg = false;
    bool mNoBorder = false;
    std::shared_ptr<Texture> mFrameCache;
    std::shared_ptr<Texture> mHoverFrameCache;
};

#endif // FRAMED_BUTTON_H
