#ifndef ETRADEPOSTINFOWIDGET_H
#define ETRADEPOSTINFOWIDGET_H

#include "eemployingbuildinginfowidget.h"

#include "../eswitchbutton.h"
#include "../espinbox.h"

#include "engine/eresourcetype.h"

#include "buildings/trade-post.h"

class TradePostInfoWidget : public eEmployingBuildingInfoWidget {
public:
    TradePostInfoWidget(eMainWindow* const window,
                         eMainWidget* const mw);

    using ePrevNextAction = std::function<void(bool)>;
    void initialize(TradePost *const stor,
                    const ePrevNextAction& prevNext);
    void get(eResourceType& imports,
             eResourceType& exports,
             eResourceType& empty,
             eResourceType& cartGet,
             eResourceType& cartAccept,
             eResourceType& cartDontAccept,
             std::map<eResourceType, int>& count) const;
private:
    std::map<eResourceType, eSwitchButton*> mImportButtons;
    std::map<eResourceType, eSwitchButton*> mExportButtons;
    std::map<eResourceType, eSwitchButton*> mCartOrderButtons;
    std::map<eResourceType, eSpinBox*> mSpinBoxes;
};

#endif // ETRADEPOSTINFOWIDGET_H
