#ifndef EGODSELECTIONWIDGET_H
#define EGODSELECTIONWIDGET_H

#include "widgets/emultipleselectionwidget.h"

#include "characters/gods/god.h"

class eGodSelectionWidget : public eMultipleSelectionWidget {
public:
    using eMultipleSelectionWidget::eMultipleSelectionWidget;

    using eGodSetAction = std::function<void(const std::vector<GodType>&)>;
    void initialize(const eGodSetAction& godSetAct,
                    const std::vector<GodType>& ini);
};

#endif // EGODSELECTIONWIDGET_H
