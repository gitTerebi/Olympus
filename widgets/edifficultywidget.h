#ifndef EDIFFICULTYWIDGET_H
#define EDIFFICULTYWIDGET_H

#include "ewidget.h"

#include "engine/edifficulty.h"

class eDifficultyWidget : public eWidget {
public:
    using eWidget::eWidget;

    using eChangeAction = std::function<void(const eDifficulty)>;
    void initialize(const eDifficulty diff, const eChangeAction& act);
};

#endif // EDIFFICULTYWIDGET_H
