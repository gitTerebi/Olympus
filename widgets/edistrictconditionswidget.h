#ifndef EDISTRICTCONDITIONSWIDGET_H
#define EDISTRICTCONDITIONSWIDGET_H

#include "escrollbuttonslist.h"

#include "engine/ai/eaidistrict.h"

class eBoardCity;

class eDistrictConditionsWidget : public eScrollButtonsList {
public:
    using eScrollButtonsList::eScrollButtonsList;

    using eConditionGetter = std::function<std::vector<eDistrictReadyCondition>()>;
    using eConditionAdder = std::function<void(const eDistrictReadyCondition&)>;
    using eConditionSetter = std::function<void(const int, const eDistrictReadyCondition&)>;
    using eConditionRemover = std::function<void(const int)>;
    void initialize(const eConditionGetter& get,
                    const eConditionAdder& add,
                    const eConditionSetter& set,
                    const eConditionRemover& remove);
};

#endif // EDISTRICTCONDITIONSWIDGET_H
