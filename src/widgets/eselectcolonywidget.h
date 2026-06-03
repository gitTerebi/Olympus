#ifndef ESELECTCOLONYWIDGET_H
#define ESELECTCOLONYWIDGET_H

#include "ewidget.h"

class WorldBoard;
class WorldCity;
struct eColonyEpisode;

class eSelectColonyWidget : public eWidget {
public:
    using eWidget::eWidget;

    using eColonySelection = std::vector<eColonyEpisode*>;
    using eCitySelected = std::function<void(const std::shared_ptr<WorldCity>)>;
    void initialize(const eColonySelection& sel,
                    const eCitySelected& s,
                    WorldBoard* const board);
};

#endif // ESELECTCOLONYWIDGET_H
