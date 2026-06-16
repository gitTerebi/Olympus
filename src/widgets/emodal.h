#ifndef EMODAL_H
#define EMODAL_H

#include "eframedwidget.h"

class eModal : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void setMask(const bool m) { mMask = m; }
    bool mask() const { return mMask; }

    void initializeMask(const int w, const int h);
    void windowSizeChanged(int w, int h) override;

    virtual void close();
    void setCloseAction(const eAction& a) { mCloseAction = a; }

    eFramedWidget* frame() const { return mFrame; }

protected:
    void resetModal();

    void paintEvent(ePainter& p) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;

private:
    bool mMask = false;
    eFramedWidget* mFrame = nullptr;
    eAction mCloseAction;
};

#endif // EMODAL_H
