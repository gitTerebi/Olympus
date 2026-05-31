#ifndef EBUTTONUTILS_H
#define EBUTTONUTILS_H

#include <string>

#include "framed-button.h"
#include "emainwindow.h"
#include "ewidget.h"

FramedButton* addPlainButton(const std::string& text,
                              const eAction& a,
                              eWidget* buttons,
                              eMainWindow* window);

FramedButton* addFramedButton(const std::string& text,
                               const eAction& a,
                               eWidget* buttons,
                              eMainWindow* window);

#endif // EBUTTONUTILS_H
