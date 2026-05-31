#ifndef EWATERTRADEWIDGET_H
#define EWATERTRADEWIDGET_H

#include "escrollbuttonslist.h"

class eWorldBoard;

enum class eCityId;

class eWaterTradeWidget : public eScrollButtonsList {
public:
    using eScrollButtonsList::eScrollButtonsList;

    using eCitiesGetter = std::function<std::vector<eCityId>()>;
    using eCityAdder = std::function<void(const eCityId)>;
    using eCityRemover = std::function<void(const eCityId)>;
    void initialize(const eCitiesGetter& get,
                    const eCityRemover& add,
                    const eCityRemover& remove,
                    eWorldBoard* const wboard);
};

#endif // EWATERTRADEWIDGET_H
