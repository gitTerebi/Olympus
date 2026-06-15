#ifndef ELABELEDWIDGET_H
#define ELABELEDWIDGET_H

#include "elabel.h"

class eLabeledWidget : public eWidget {
public:
    eLabeledWidget(MainWindow* const window);

    void setup(const std::string& label, eWidget* const w);
};

#endif // ELABELEDWIDGET_H
