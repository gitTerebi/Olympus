#ifndef EEPISODEGOALWIDGET_H
#define EEPISODEGOALWIDGET_H

#include "eframedwidget.h"

template <class T> using stdsptr = std::shared_ptr<T>;

class eEpisodeGoal;
class eGameBoard;

class eEpisodeGoalWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize(const stdsptr<eEpisodeGoal>& e,
                    eGameBoard* const board);
};

#endif // EEPISODEGOALWIDGET_H
