#ifndef ECITYONBOARDSELECTIONWIDGET_H
#define ECITYONBOARDSELECTIONWIDGET_H

#include "escrollbuttonslist.h"

#include "pointers/estdselfref.h"

class eBoardCity;
class eWorldBoard;
class eGameBoard;

enum class eCityId;

enum class eGameEventBranch;
class eCityOnBoardSelectionWidget : public eScrollButtonsList {
public:
    using eScrollButtonsList::eScrollButtonsList;

    using eCitiesGetter = std::function<std::vector<eCityId>()>;
    using eCityAdder = std::function<void(const eCityId)>;
    using eCityRemover = std::function<void(const eCityId)>;
    void initialize(const eCitiesGetter& get,
                    const eCityAdder& add,
                    const eCityRemover& remove,
                    eGameBoard* const board,
                    eWorldBoard* const wboard);
};

#endif // ECITYONBOARDSELECTIONWIDGET_H
