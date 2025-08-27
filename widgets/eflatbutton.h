#ifndef EFLATBUTTON_H
#define EFLATBUTTON_H

#include "ebuttonbase.h"

class eFlatButton : public eButtonBase {
public:
    using eButtonBase::eButtonBase;
protected:
    void sizeHint(int& w, int& h) override;
    void paintEvent(ePainter& p) override;
};

#endif // EFLATBUTTON_H
