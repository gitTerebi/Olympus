#ifndef EDIFFICULTYWIDGET_H
#define EDIFFICULTYWIDGET_H

#include "ewidget.h"

#include "engine/difficulty.h"

class eDifficultyWidget : public eWidget {
public:
    using eWidget::eWidget;

    using eChangeAction = std::function<void(const Difficulty)>;
    void initialize(const Difficulty diff, const eChangeAction& act);
};

#endif // EDIFFICULTYWIDGET_H
