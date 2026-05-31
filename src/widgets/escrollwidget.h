#ifndef ESCROLLWIDGET_H
#define ESCROLLWIDGET_H

#include "ewidget.h"
#include "ebuttonbase.h"

class eScrollWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initializeButtons();

    void setScrollArea(eWidget* const w);
    eWidget* scrollArea() const { return mScrollArea; }

    void scrollUp();
    void scrollDown();
    void scrollToTheTop();
    void clampDY();

    void renderTargetsReset() override;
protected:
    void paintEvent(ePainter& p) override;

    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseWheelEvent(const eMouseWheelEvent& e) override;
private:
    int mDy = 0;

    eWidget* mScrollArea = nullptr;
    eButtonBase* mUpButton = nullptr;
    eButtonBase* mDownButton = nullptr;
};

#endif // ESCROLLWIDGET_H
