#ifndef EBUTTONUTILS_H
#define EBUTTONUTILS_H

#include <string>

#include "framed-button.h"
#include "main-window.h"
#include "ewidget.h"

FramedButton* addPlainButton(const std::string& text,
                              const eAction& a,
                              eWidget* buttons,
                              MainWindow* window);

FramedButton* addFramedButton(const std::string& text,
                               const eAction& a,
                               eWidget* buttons,
                              MainWindow* window);

#endif // EBUTTONUTILS_H
