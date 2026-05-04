#ifndef EPOPUPWIDGET_H
#define EPOPUPWIDGET_H

#include "ewidget.h"

class eFramedWidget;

class ePopupWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int w, const int h);

    eFramedWidget* frame() const { return mFrame; }
    void setCloseAction(const eAction& a) { mCloseAction = a; }

protected:
    void paintEvent(ePainter& p) override;
    virtual void closePopup();
    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;

private:
    eFramedWidget* mFrame = nullptr;
    eAction mCloseAction;
};

#endif // EPOPUPWIDGET_H
