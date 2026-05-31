#include "epyramidselectionwidget.h"

#include "widgets/echoosebutton.h"
#include "emainwindow.h"
#include "engine/eepisodegoal.h"
#include "epyramidwidget.h"

void ePyramidSelectionWidget::initialize(
        const ePyramidsGetter& get,
        const ePyramidAdder& add,
        const ePyramidRemover& remove,
        const eAction& updater) {

    eScrollButtonsList::initialize();

    const auto iniEs = get();
    for(const auto& e : iniEs) {
        const auto eStr = eBuilding::sNameForBuilding(e->fType);
        addButton(eStr);
    }

    const auto editPyramid = [this, add, remove, updater](
                             const ePSptr& e) {
        const auto settings = new ePyramidWidget(window());
        settings->resize(2*width()/3, 2*height()/3);
        settings->initialize(e, updater);

        window()->execDialog(settings);
        settings->align(eAlignment::center);
    };

    setButtonPressEvent([get, editPyramid](const int id) {
        const auto es = get();
        const auto& e = es[id];
        editPyramid(e);
    });

    setButtonCreateEvent([this, add, editPyramid]() {
        const std::vector<eBuildingType> types = {
            eBuildingType::modestPyramid, // 3x3
            eBuildingType::pyramid, // 5x5
            eBuildingType::greatPyramid, // 7x7
            eBuildingType::majesticPyramid, // 9x9

            eBuildingType::smallMonumentToTheSky, // 5x5
            eBuildingType::monumentToTheSky, // 6x6
            eBuildingType::grandMonumentToTheSky, // 8x8

            eBuildingType::minorShrineAphrodite, // 3x3
            eBuildingType::shrineAphrodite, // 6x6
            eBuildingType::majorShrineAphrodite, // 8x8

            eBuildingType::pyramidOfThePantheon, // 11x9
            eBuildingType::altarOfOlympus, // 8x8
            eBuildingType::templeOfOlympus, // 8x8
            eBuildingType::observatoryKosmika, // 9x9
            eBuildingType::museumAtlantika // 8x8
        };
        std::vector<std::string> labels;
        for(const auto t : types) {
            const auto eStr = eBuilding::sNameForBuilding(t);
            labels.push_back(eStr);
        }
        const auto echoose = new eChooseButton(window());
        const auto act = [this, add, types, labels, editPyramid](const int val) {
            const auto type = types[val];
            const auto e = std::make_shared<ePyramidSettings>();
            e->fType = type;
            const int nlevels = ePyramid::sLevels(type);
            for(int i = 0; i < nlevels; i++) {
                e->fLevels.push_back(i % 2);
            }

            add(e);
            editPyramid(e);
            const auto eStr = eBuilding::sNameForBuilding(type);
            addButton(eStr);
        };
        echoose->initialize(8, labels, act);

        window()->execDialog(echoose);
        echoose->align(eAlignment::center);
    });

    setButtonRemoveEvent([get, remove](const int id) {
        const auto es = get();
        const auto& e = es[id];
        remove(e);
    });
}
