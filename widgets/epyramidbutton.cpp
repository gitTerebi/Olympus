#include "epyramidbutton.h"

#include "echoosebutton.h"
#include "emainwindow.h"
#include "elanguage.h"

void ePyramidButton::initialize(const ePyramidAction& pact) {
    setUnderline(false);
    setPressAction([this, pact]() {
        const auto choose = new eChooseButton(window());
        const std::vector<eBuildingType> pyramids = {
            eBuildingType::none,

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
        std::vector<std::string> pyramidNames;
        for(const auto type : pyramids) {
            if(type == eBuildingType::none) {
                pyramidNames.push_back(eLanguage::zeusText(194, 39));
            } else {
                pyramidNames.push_back(eBuilding::sNameForBuilding(type));
            }
        }

        const auto act = [this, pyramids, pact](const int val) {
            const auto t = pyramids[val];
            setType(t);
            if(pact) pact(t);
        };
        choose->initialize(7, pyramidNames, act);

        window()->execDialog(choose);
        choose->align(eAlignment::center);
    });
    setType(eBuildingType::grandMonumentToTheSky);
    fitContent();
}

void ePyramidButton::setType(const eBuildingType type) {
    if(type == eBuildingType::none) {
        setText(eLanguage::zeusText(194, 39));
    } else {
        setText(eBuilding::sNameForBuilding(type));
    }
    mType = type;
}
