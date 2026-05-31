#ifndef EBUTTONUTILS_H
#define EBUTTONUTILS_H

#include <string>

#include "eframedbutton.h"
#include "emainwindow.h"
#include "ewidget.h"

eFramedButton* addPlainButton(const std::string& text,
                              const eAction& a,
                              eWidget* buttons,
                              eMainWindow* window);

eFramedButton* addFramedButton(const std::string& text,
                               const eAction& a,
                               eWidget* buttons,
                              eMainWindow* window);

#endif // EBUTTONUTILS_H
