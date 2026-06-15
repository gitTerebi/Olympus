#ifndef EQUESTIONWIDGET_H
#define EQUESTIONWIDGET_H

#include "eframedwidget.h"
#include "pointers/estdselfref.h"

class Texture;

class eQuestionWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const std::string& title,
                    const std::string& text,
                    const eAction& acceptA,
                    const eAction& cancelA);
    void initialize(const std::string& title,
                    const std::string& text,
                    const stdsptr<Texture>& icon,
                    const eAction& acceptA,
                    const eAction& cancelA);
};

#endif // EQUESTIONWIDGET_H
