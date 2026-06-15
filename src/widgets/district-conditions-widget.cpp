#include "district-conditions-widget.h"

#include "widgets/echoosebutton.h"
#include "main-window.h"
#include "engine/board-city.h"
#include "ecitysettingswidget.h"
#include "engine/game-board.h"
#include "ecitybutton.h"
#include "echoosecitydialog.h"
#include "condition-settings-widget.h"

void DistrictConditionsWidget::initialize(
        const eConditionGetter& get,
        const eConditionAdder& add,
        const eConditionSetter& set,
        const eConditionRemover& remove) {
    eScrollButtonsList::initialize();

    const auto iniCs = get();
    for(const auto& c : iniCs) {
        addButton(c.name());
    }

    const auto editCond = [this, get, set](const int id) {
        const auto got = get();
        const int n = got.size();
        if(id >= n) return;
        const auto setter = [this, set, id](const eDistrictReadyCondition& c) {
            set(id, c);
            setText(id, c.name());
        };
        const auto settings = new ConditionSettingsWidget(window());
        settings->resize(2*width()/3, 2*height()/3);
        const auto ini = got[id];
        settings->initialize(ini, setter);

        window()->execDialog(settings);
        settings->align(Alignment::center);
    };

    setButtonPressEvent([get, editCond](const int id) {
        editCond(id);
    });

    setButtonCreateEvent([this, add, get, editCond]() {
        const int id = get().size();
        eDistrictReadyCondition cond;
        cond.fType = eDistrictReadyCondition::eType::districtResourceCount;
        cond.fResource = eResourceType::food;
        cond.fValue = 16;
        add(cond);
        editCond(id);
        addButton(cond.name());
    });

    setButtonRemoveEvent(remove);
}

