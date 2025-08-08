#ifndef EPYRAMIDSELECTIONWIDGET_H
#define EPYRAMIDSELECTIONWIDGET_H

#include "escrollbuttonslist.h"

#include "pointers/estdselfref.h"

#include "buildings/pyramids/epyramid.h"

class ePyramidSelectionWidget : public eScrollButtonsList {
public:
    using eScrollButtonsList::eScrollButtonsList;

    using ePSptr = std::shared_ptr<ePyramidSettings>;
    using ePyramidsGetter = std::function<std::vector<ePSptr>()>;
    using ePyramidAdder = std::function<void(const ePSptr&)>;
    using ePyramidRemover = std::function<void(const ePSptr&)>;
    void initialize(const ePyramidsGetter& get,
                    const ePyramidAdder& add,
                    const ePyramidRemover& remove,
                    const eAction& updater);
};

#endif // EPYRAMIDSELECTIONWIDGET_H
