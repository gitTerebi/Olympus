#include "econditionsettingswidget.h"

#include "eresourcebutton.h"
#include "egodbutton.h"
#include "evaluebutton.h"

#include "echoosebutton.h"
#include "emainwindow.h"

class eConditionTypeButton : public eFramedButton {
public:
    using eFramedButton::eFramedButton;

    using eType = eDistrictReadyCondition::eType;
    using eCondAction = std::function<void(const eType)>;
    void initialize(const eCondAction& gact);

    eType type() const { return mType; }
    void setType(const eType type);
private:
    eType mType;
};

void eConditionTypeButton::initialize(const eCondAction& gact) {
    setUnderline(false);
    setPressAction([this, gact]() {
        const auto choose = new eChooseButton(window());
        std::vector<eType> types;
        std::vector<std::string> names;
        const int iMax = static_cast<int>(eType::count);
        for(int i = 0; i < iMax; i++) {
            const auto type = static_cast<eType>(i);
            types.push_back(type);
            names.push_back(eDistrictReadyCondition::sName(type));
        }

        const auto act = [this, types, gact](const int val) {
            const auto t = types[val];
            setType(t);
            if(gact) gact(t);
        };
        choose->initialize(7, names, act);

        window()->execDialog(choose);
        choose->align(eAlignment::center);
    });
    setType(eType::districtResourceCount);
    fitContent();
}

void eConditionTypeButton::setType(const eType type) {
    setText(eDistrictReadyCondition::sName(type));
    mType = type;
}

void eConditionSettingsWidget::initialize(
        const eDistrictReadyCondition& ini,
        const eSetter& setter) {
    const auto value = std::make_shared<eDistrictReadyCondition>(ini);

    setType(eFrameType::message);
    const int p = padding();

    const auto innerW = new eWidget(window());

    const auto typeButton = new eConditionTypeButton(window());
    using eType = eDistrictReadyCondition::eType;
    typeButton->initialize([setter, value](const eType type) {
        value->fType = type;
        setter(*value);
    });
    typeButton->setType(ini.fType);
    innerW->addWidget(typeButton);

    const auto godButton = new eGodButton(window());
    godButton->initialize([setter, value](const eGodType type) {
        value->fSanctuary = type;
        setter(*value);
    });
    godButton->setType(ini.fSanctuary);
    innerW->addWidget(godButton);

    const auto resButton = new eResourceButton(window());
    resButton->initialize([setter, value](const eResourceType type) {
        value->fResource = type;
        setter(*value);
    }, eResourceType::allBasic | eResourceType::drachmas, true);
    resButton->setResource(ini.fResource);
    innerW->addWidget(resButton);

    const auto valButton = new eValueButton(window());
    valButton->initialize(0, 99999);
    valButton->setValue(ini.fValue);
    valButton->setValueChangeAction([setter, value](const int v) {
        value->fValue = v;
        setter(*value);
    });
    innerW->addWidget(valButton);

    innerW->stackVertically(p);

    addWidget(innerW);
    innerW->fitContent();
    innerW->move(p, p);
    resize(innerW->width() + 2*p,
           innerW->height() + 2*p);
}
