#ifndef EEVENTBACKGROUND_H
#define EEVENTBACKGROUND_H

#include "ewidget.h"

class eEventBackground : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(eWidget* const parent,
                    eWidget* const child,
                    const bool closable,
                    const eAction& closeFunc);
    void windowSizeChanged(int w, int h) override;
protected:
    void paintEvent(ePainter& p) override;
    bool keyPressEvent(const eKeyPressEvent& e) override;
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;
private:
    void close();

    bool mClosable = false;
    eAction mCloseFunc;
};

#endif // EEVENTBACKGROUND_H
